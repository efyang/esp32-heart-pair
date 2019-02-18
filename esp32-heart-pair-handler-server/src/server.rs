use uuid::Uuid;
use failure::Error;

use std::io::ErrorKind;
use std::net::{UdpSocket, SocketAddr, IpAddr};
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

impl Server {
    pub fn new(port: u16) -> Result<Server, Error> {
        let socket = UdpSocket::bind(&SocketAddr::new("127.0.0.1".parse()?, port))?;
        socket.set_nonblocking(true)?;
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
        }
    }

    fn message_in_order(&mut self, address: SocketAddr, send_time: usize) -> bool {
        unimplemented!()
    }

    fn handle_datagram(&mut self) -> Result<(), Error> {
        match self.socket.recv_from(&mut self.buffer) {
            Ok((bytes_read, address)) => {
                let (message, send_time) = parse_datagram(&self.buffer[..bytes_read])?;
                if !self.last_message_times.contains_key(&address) {
                    self.last_message_times.insert(address, 0);
                }

                if self.message_in_order(address, send_time) {
                    self.last_message_times.insert(address, send_time);
                    self.handle_message(message, address)?;
                } // otherwise we throw it out
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
            RecvMessage::Trigger => {
                self.trigger_state = true;
            }
            RecvMessage::Stop => {
                self.trigger_state = false;
            }
            RecvMessage::GotUpdate(uuid) => {

            }
            RecvMessage::ClientUpdate(bpm, elapsed_since_beat_peak) => {

            }
        }

        Ok(())
    }


    // we only add a client if it sends us GOT_UPDATE or CLIENT_UPDATE messages - TRIGGER and
    // STOP_TRIGGER are separate and distinct so we never confuse client and trigger messages
    fn add_client(&mut self) {

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

struct PairClient {
    latency: Duration,
    await_ack_message_queue: HashMap<Uuid, SystemTime>,
    should_send_trigger_state: bool,
}

const AWAIT_ACK_MAX_TIME: Duration = Duration::from_millis(500);
impl PairClient {
    fn new(should_send_trigger_state: bool, last_message_time: usize) -> PairClient {
        PairClient {
            latency: Duration::from_millis(0),
            await_ack_message_queue: HashMap::new(),
            should_send_trigger_state: should_send_trigger_state,
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
                                       LATENCY_COMP_FILTER_NEW * new_latency.as_millis() as f64) as u64)
            });
        }
    }
}
