use uuid::Uuid;
use failure::Error;

use std::time::{SystemTime, Duration};
use std::ffi::CStr;

pub enum RecvMessage {
    Trigger,
    Stop,
    // uuid
    GotUpdate(Uuid),
    // bitflagged (love, happy, sad, fear, anger)
    ClientUpdate(bool, bool, bool, bool, bool),
}

pub enum SendMessage {
    // send time, uuid, bitflagged (love, happy, sad, fear, anger), trigger state, estimated latency
    ServerUpdate(SystemTime, Uuid, bool, bool, bool, bool, bool, bool, Duration),
}

fn to_mood_bits(love: bool, happy: bool, sad: bool, fear: bool, anger: bool) -> u8 {
    ((love as u8) << 4 ) | ((happy as u8) << 3) | ((sad as u8) << 2) | ((fear as u8) << 1) | ((anger as u8) << 0)
}

fn nth_bit_set(bits: u8, n: usize) -> bool {
    (bits >> n) & 1 == 1
}

fn from_mood_bits(bits: u8) -> (bool, bool, bool, bool, bool) {
    (nth_bit_set(bits, 4), nth_bit_set(bits, 3), nth_bit_set(bits, 2), nth_bit_set(bits, 1), nth_bit_set(bits, 0))
}

impl SendMessage {
    pub fn serialize(self) -> Result<String, Error> {
        match self {
            SendMessage::ServerUpdate(send_time,
                                      uuid,
                                      love_mood,
                                      happy_mood,
                                      sad_mood,
                                      fear_mood,
                                      anger_mood,
                                      trigger_state,
                                      estimated_latency) => {
                Ok(format!("SERVER_UPDATE {} {} {} {} {}",
                           send_time.duration_since(SystemTime::UNIX_EPOCH)?.as_millis(),
                           uuid,
                           to_mood_bits(love_mood, happy_mood, sad_mood, fear_mood, anger_mood),
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
            let mood_bits = split_s[2].parse::<u8>().unwrap();
            let (a,b,c,d,e) = from_mood_bits(mood_bits);
            Ok((RecvMessage::ClientUpdate(a,b,c,d,e), time))
        }
        _ => {
            bail!("Faulty Datagram")
        }
    }
}
