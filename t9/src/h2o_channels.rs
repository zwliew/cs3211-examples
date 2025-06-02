use std::sync::Arc;

use futures::future::join_all;
use tokio::sync::{mpsc, Barrier, Mutex, Semaphore};

async fn hydrogen(
    id: usize,
    barrier: Arc<Barrier>,
    sem: Arc<Semaphore>,
    chan: mpsc::Sender<usize>,
) {
    let _permit = sem.acquire().await.unwrap();
    barrier.wait().await;

    chan.send(id).await.unwrap();
}

async fn oxygen(id: usize, barrier: Arc<Barrier>, chan: Arc<Mutex<mpsc::Receiver<usize>>>) {
    let mut chan_guard = chan.lock().await;
    barrier.wait().await;

    let h1 = chan_guard.recv().await.unwrap();
    let h2 = chan_guard.recv().await.unwrap();
    println!("H {} - O {} - H {}", h1, id, h2);
}

#[tokio::main]
pub async fn main() -> std::io::Result<()> {
    let barrier = Arc::new(Barrier::new(3));
    let h_sem = Arc::new(Semaphore::new(2));

    let (s, r) = mpsc::channel(2);
    let r = Arc::new(Mutex::new(r));

    let hydrogens =
        (0..200).map(|i| tokio::spawn(hydrogen(i, barrier.clone(), h_sem.clone(), s.clone())));
    let oxygens = (0..100).map(|i| tokio::spawn(oxygen(i, barrier.clone(), r.clone())));

    let join_handles = Iterator::chain(hydrogens, oxygens).collect::<Vec<_>>();

    std::mem::drop(s);
    std::mem::drop(r);

    join_all(join_handles).await;

    Ok(())
}
