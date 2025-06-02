use mio::event::Event;
use mio::net::{TcpListener, TcpStream};
use mio::{Events, Interest, Poll, Registry, Token};

use std::collections::HashMap;
use std::io::{BufRead, BufReader, ErrorKind, Write};
use std::net::SocketAddr;

const SERVER: Token = Token(0);

struct ClientState {
    reader: BufReader<TcpStream>,
    line_buffer: Vec<u8>,
    written_until: usize,
    read_complete: bool,
}

pub fn main() -> std::io::Result<()> {
    let mut poll = Poll::new()?;
    let mut events = Events::with_capacity(128);

    // Setup the TCP server socket.
    let port = std::env::args()
        .nth(1)
        .map(|s| s.parse().unwrap())
        .unwrap_or(50000u16);
    let mut listener = TcpListener::bind(SocketAddr::from(([127, 0, 0, 1], port)))?;

    // Register the server with poll we can receive events for it.
    poll.registry()
        .register(&mut listener, SERVER, Interest::READABLE)?;

    // Map of `Token` -> `ClientState`.
    let mut connections = HashMap::new();
    // Unique token for each incoming connection.
    let mut next_token = Token(SERVER.0 + 1);

    loop {
        poll.poll(&mut events, None)?;

        for event in events.iter() {
            match event.token() {
                SERVER => loop {
                    // Received an event for the TCP server socket, which
                    // indicates we can accept an connection.
                    let (mut connection, address) = match listener.accept() {
                        Ok((connection, address)) => (connection, address),
                        Err(e) if e.kind() == ErrorKind::WouldBlock => {
                            // If we get a `WouldBlock` error we know our
                            // listener has no more incoming connections queued,
                            // so we can return to polling and wait for some
                            // more.
                            break;
                        }
                        Err(e) => {
                            // If it was any other kind of error, something went
                            // wrong and we terminate with an error.
                            return Err(e);
                        }
                    };

                    println!("Accepted connection from: {}", address);
                    let token = next_token;
                    next_token.0 += 1;
                    poll.registry()
                        .register(&mut connection, token, Interest::READABLE)?;
                    connections.insert(
                        token,
                        ClientState {
                            reader: BufReader::new(connection),
                            line_buffer: Vec::new(),
                            written_until: 0,
                            read_complete: false,
                        },
                    );
                },
                token => {
                    // Maybe received an event for a TCP connection.
                    let done = if let Some(connection) = connections.get_mut(&token) {
                        handle_connection_event(poll.registry(), connection, event)?
                    } else {
                        // Sporadic events happen, we can safely ignore them.
                        false
                    };
                    if done {
                        if let Some(mut connection) = connections.remove(&token) {
                            poll.registry().deregister(connection.reader.get_mut())?;
                        }
                    }
                }
            }
        }
    }
}

/// Returns `true` if the connection is done.
fn handle_connection_event(
    registry: &Registry,
    connection: &mut ClientState,
    event: &Event,
) -> std::io::Result<bool> {
    if connection.read_complete && event.is_writable() {
        // We can (maybe) write to the connection.
        match connection
            .reader
            .get_mut()
            .write(&connection.line_buffer[connection.written_until..])
        {
            Ok(n) => {
                connection.written_until += n;
                if connection.written_until >= connection.line_buffer.len() {
                    // After we've written a full line, reset the buffer and go back to reading.
                    connection.line_buffer.clear();
                    connection.written_until = 0;
                    connection.read_complete = false;
                    registry.reregister(
                        connection.reader.get_mut(),
                        event.token(),
                        Interest::READABLE,
                    )?;
                }
            }
            // Would block "errors" are the OS's way of saying that the
            // connection is not actually ready to perform this I/O operation.
            Err(ref err) if err.kind() == ErrorKind::WouldBlock => {}
            // Got interrupted (how rude!), we'll try again.
            Err(ref err) if err.kind() == ErrorKind::Interrupted => {
                return handle_connection_event(registry, connection, event)
            }
            // Other errors we'll consider fatal.
            Err(err) => return Err(err),
        }
    }
    if !connection.read_complete && event.is_readable() {
        // We can (maybe) read from the connection.
        match connection
            .reader
            .read_until(b'\n', &mut connection.line_buffer)
        {
            Ok(0) => {
                // Reading 0 bytes means the other side has closed the
                // connection or is done writing, then so are we.
                println!("Connection closed");
                return Ok(true);
            }
            Ok(_) => {
                connection.read_complete = true;
                registry.reregister(
                    connection.reader.get_mut(),
                    event.token(),
                    Interest::WRITABLE,
                )?;
            }
            // Would block "errors" are the OS's way of saying that the
            // connection is not actually ready to perform this I/O operation.
            Err(ref err) if err.kind() == ErrorKind::WouldBlock => {}
            Err(ref err) if err.kind() == ErrorKind::Interrupted => {
                return handle_connection_event(registry, connection, event)
            }
            // Other errors we'll consider fatal.
            Err(err) => return Err(err),
        }
    }

    Ok(false)
}
