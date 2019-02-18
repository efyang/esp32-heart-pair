extern crate uuid;
extern crate failure;

use failure::Error;

mod message;
mod server;

fn main() -> Result<(), Error> {
    use std::env;

    let port = env::args().nth(1).unwrap_or("8080".to_string()).parse().unwrap();
    let mut server = server::Server::new(port)?;
    server.run()?;

    Ok(())
}
