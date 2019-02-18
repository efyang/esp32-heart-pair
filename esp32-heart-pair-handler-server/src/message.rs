use uuid::Uuid;
use failure::Error;

use std::time::{SystemTime, Duration};
use std::ffi::CStr;

pub enum RecvMessage {
    Trigger,
    Stop,
    // uuid
    GotUpdate(Uuid),
    // bpm, time since beat peak
    ClientUpdate(f64, Duration),
}

pub enum SendMessage {
    // send time, uuid, bpm, time since beat peak, trigger state, estimated latency
    ServerUpdate(SystemTime, Uuid, f64, Duration, bool, Duration),
}

impl SendMessage {
    pub fn serialize(self) -> Result<String, Error> {
        match self {
            SendMessage::ServerUpdate(send_time, uuid, bpm, elapsed_since_beat_peak, trigger_state, estimated_latency) => {
                Ok(format!("SERVER_UPDATE {} {} {} {} {} {}",
                           send_time.elapsed()?.as_millis(),
                           uuid,
                           bpm,
                           elapsed_since_beat_peak.as_millis(),
                           trigger_state,
                           estimated_latency.as_millis()))
            }
        }
    }
}

// TODO
// message, send time
pub fn parse_datagram(datagram: &[u8]) -> Result<(RecvMessage, usize), Error> {
    let datagram_s = CStr::from_bytes_with_nul(datagram)?.to_str()?.trim().to_owned();
    println!("Got string: {}", datagram_s);
    let split_s = datagram_s.split(" ").collect::<Vec<&str>>();
    println!("strings: {:?}", split_s);
    let time: usize = split_s[1].parse()?;
    match split_s[0] {
        "TRIGGER" => {
            Ok((RecvMessage::Trigger, time))
        }
        "STOP" => {
            Ok((RecvMessage::Stop, time))
        }
        "GOT_UPDATE" => {
            let uuid = Uuid::parse_str(split_s[2])?;
            Ok((RecvMessage::GotUpdate(uuid), time))
        }
        "CLIENT_UPDATE" => {
            let bpm: f64 = split_s[2].parse()?;
            let elapsed_since_beat_peak: u64 = split_s[3].parse()?;
            Ok((RecvMessage::ClientUpdate(bpm, Duration::from_millis(elapsed_since_beat_peak)), time))
        }
        _ => {
            bail!("Faulty Datagram")
        }
    }
}
