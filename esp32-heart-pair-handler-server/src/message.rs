use uuid::Uuid;
use failure::Error;

use std::time::{SystemTime, Duration};

pub enum RecvMessage {
    Trigger,
    Stop,
    // uuid
    GotUpdate(Uuid),
    // bpm, time since beat peak
    ClientUpdate(f64, Duration),
}

pub enum SendMessage {
    // uuid, bpm, time since beat peak, trigger state, estimated latency, send time
    ServerUpdate(Uuid, f64, Duration, bool, Duration, usize, SystemTime),
}

impl SendMessage {
    pub fn serialize(self) -> String {
        unimplemented!()
    }
}

// TODO
// message, send time
pub fn parse_datagram(datagram: &[u8]) -> Result<(RecvMessage, usize), Error> {
    unimplemented!()
}
