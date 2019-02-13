use uuid::Uuid;

pub enum MessageType {
    Trigger,
    Stop,
    ServerUpdate(Uuid),
    GotUpdate(Uuid),
    ClientUpdate,
}
