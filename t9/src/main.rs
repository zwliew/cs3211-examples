mod async_tokio;
mod h2o;
mod h2o_channels;
mod raw_mio;
mod threaded;

fn main() -> std::io::Result<()> {
    // threaded::main()
    // async_tokio::main()
    h2o::main()
    // h2o_channels::main()
    // raw_mio::main()
}
