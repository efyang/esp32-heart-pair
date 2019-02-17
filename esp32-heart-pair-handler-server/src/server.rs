use tokio::io;
use tokio::net::UdpSocket;
use tokio::prelude::*;

use uuid::Uuid;

use std::net::{SocketAddr, IpAddr};
use std::collections::HashMap;
use std::time::{SystemTime, Duration};

struct Server {
    socket: UdpSocket,
    clients: HashMap<SocketAddr, PairClient>, // in ms
}

impl Server {
    fn new(port: u16) -> Server {
        Server {
            socket: UdpSocket::bind(&SocketAddr::new("127.0.0.1".parse().unwrap(), port)).unwrap(),
            clients: HashMap::new(),
        }
    }

    fn run(&mut self) {

    }

    // we only add a client if it sends us GOT_UPDATE or CLIENT_UPDATE messages - TRIGGER and
    // STOP_TRIGGER are separate and distinct so we never confuse client and trigger messages
    fn add_client(&mut self) {

    }
}


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
