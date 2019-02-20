use uuid::Uuid;
use failure::Error;

use std::ffi::CString;
use std::io::ErrorKind;
use std::net::{UdpSocket, SocketAddr};
use std::collections::HashMap;
use std::time::{SystemTime, Duration};

use crate::message::{parse_datagram, RecvMessage, SendMessage};

const DATAGRAM_MAX_SIZE: usize = 65_507;
pub struct Server {
    socket: UdpSocket,
    clients: HashMap<SocketAddr, PairClient>, // in ms
    // some monotonically increasing value that is initially determined by the sender
    // used to solve out-of-order
    last_message_times: HashMap<SocketAddr, usize>,
    last_clean_time: SystemTime,
    buffer: [u8; DATAGRAM_MAX_SIZE],
    trigger_state: bool,
}

const SERVER_LOOP_DELAY: Duration = Duration::from_millis(2);
const SERVER_BIND_IP: &'static str = "0.0.0.0";
impl Server {
    pub fn new(port: u16) -> Result<Server, Error> {
        let socket = UdpSocket::bind(&SocketAddr::new(SERVER_BIND_IP.parse()?, port))?;
        socket.set_nonblocking(true)?;
        println!("Server running on: {}", socket.local_addr()?);
        Ok(Server {
            socket: socket,
            clients: HashMap::new(),
            last_message_times: HashMap::new(),
            last_clean_time: SystemTime::now(),
            buffer: [0; DATAGRAM_MAX_SIZE],
            trigger_state: false,
        })
    }

    // handle any incoming datagrams and clean the queue
    pub fn run(&mut self) -> Result<(), Error> {
        loop {
            self.handle_datagram()?;
            self.clean_queue();
            ::std::thread::sleep(SERVER_LOOP_DELAY);
        }
    }

    fn message_in_order(&mut self, address: SocketAddr, send_time: usize) -> bool {
        if !self.last_message_times.contains_key(&address) {
            self.last_message_times.insert(address, send_time);
            return true;
        } else {
            return send_time > self.last_message_times[&address];
        }
    }

    fn handle_datagram(&mut self) -> Result<(), Error> {
        // clear the buffer
        self.buffer = [0; DATAGRAM_MAX_SIZE];
        match self.socket.recv_from(&mut self.buffer) {
            Ok((bytes_read, address)) => {
                println!("GOT {} bytes from address: {}", bytes_read, address);
                // 1 extra byte to nul terminate
                let (message, send_time) = parse_datagram(&self.buffer[..bytes_read + 1])?;

                // we only want messages that are in order
                if self.message_in_order(address, send_time) {
                    // update with the new message times
                    self.last_message_times.insert(address, send_time);
                    self.handle_message(message, address)?;
                } // otherwise we throw it out

                println!("HANDLED DATAGRAM... NEW STATE:");
                dbg!(&self.clients);
                dbg!(&self.last_message_times);
                dbg!(&self.last_clean_time);
                dbg!(self.trigger_state);
                println!("");
            }
            Err(ref e) if e.kind() == ErrorKind::WouldBlock => {},
            Err(e) => {
                Err(e)?;
            }
        }

        Ok(())
    }

    fn handle_message(&mut self, message: RecvMessage, address: SocketAddr) -> Result<(), Error> {
        match message {
            // TODO: Forward triggers?
            RecvMessage::Trigger => {
                self.trigger_state = true;
            }
            RecvMessage::Stop => {
                self.trigger_state = false;
            }
            RecvMessage::GotUpdate(uuid) => {
                let current_time = SystemTime::now();
                self.add_client(&address);
                self.clients.get_mut(&address).unwrap().recalculate_latency_got_ack(uuid, current_time);
            }
            RecvMessage::ClientUpdate(bpm, elapsed_since_beat_peak) => {
                let current_time = SystemTime::now();
                let uuid = Uuid::new_v4();
                self.add_client(&address);
                let forward = SendMessage::ServerUpdate(current_time, uuid, bpm, elapsed_since_beat_peak, self.trigger_state, self.clients[&address].latency());
                let forward_s = CString::new(forward.serialize()?)?;
                for (client_address, client_info) in self.clients.iter_mut().filter(|(&k, _)| k != address) {
                    client_info.add_message(uuid, current_time);
                    self.socket.send_to(forward_s.as_bytes_with_nul(), client_address)?;
                }
            }
        }

        Ok(())
    }


    // we only add a client if it sends us GOT_UPDATE or CLIENT_UPDATE messages - TRIGGER and
    // STOP_TRIGGER are separate and distinct so we never confuse client and trigger messages
    fn add_client(&mut self, address: &SocketAddr) {
        if !self.clients.contains_key(address) {
            self.clients.insert(address.clone(), PairClient::new());
        }
    }

    fn clean_queue(&mut self) {
        let current_time = SystemTime::now();
        // if we have waited AWAIT_ACK_MAX_TIME since last clean
        if current_time.duration_since(self.last_clean_time).ok().map_or(false, |t| t > AWAIT_ACK_MAX_TIME) {
            for client_info in self.clients.values_mut() {
                client_info.clean_messages();
            }
        }
    }
}

#[derive(Debug)]
struct PairClient {
    latency: Duration,
    await_ack_message_queue: HashMap<Uuid, SystemTime>,
}

const AWAIT_ACK_MAX_TIME: Duration = Duration::from_millis(500);
impl PairClient {
    fn new() -> PairClient {
        PairClient {
            latency: Duration::from_millis(0),
            await_ack_message_queue: HashMap::new(),
        }
    }

    // add a message to the await ack queue
    fn add_message(&mut self, uuid: Uuid, time: SystemTime) {
        self.await_ack_message_queue.insert(uuid, time);
    }

    // clean out all old messages
    fn clean_messages(&mut self) {
        let current_time = SystemTime::now();
        // only keep the messages that are within the max ack time
        self.await_ack_message_queue.retain(move |_, &mut v| {
            // check if duration is less than current time
            // if not ok, means the time was actually later than the current time, so default to
            // true
            current_time.duration_since(v).ok().map_or(true, |t| t < AWAIT_ACK_MAX_TIME)
        });
    }

    fn latency(&self) -> Duration {
        self.latency
    }
}

const LATENCY_COMP_FILTER_PREV: f64 = 0.9;
const LATENCY_COMP_FILTER_NEW: f64 = 1f64- LATENCY_COMP_FILTER_PREV;
impl PairClient {
    // recalculate latency with info from a new ack
    fn recalculate_latency_got_ack(&mut self, uuid: Uuid, ack_time: SystemTime) {
        if let Some(time) = self.await_ack_message_queue.remove(&uuid) {
            // use a complementary filter to calculate the latency
            self.latency = ack_time.duration_since(time).ok().map_or(self.latency, |new_latency| {
                Duration::from_millis((LATENCY_COMP_FILTER_PREV * self.latency.as_millis() as f64 +
                                       LATENCY_COMP_FILTER_NEW * (new_latency.as_millis() as f64 / 2f64)) as u64)
                    // new latency is 1/2 of round-trip time
            });
        } else {
            eprintln!("ack without existing message");
        }
    }
}
