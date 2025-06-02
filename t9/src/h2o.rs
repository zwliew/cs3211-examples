use futures::future;
use std::sync::Arc;
use tokio::sync::{Barrier, Semaphore};

fn bond(s: &str) {
    println!("bond {}", s);
}

#[tokio::main]
pub async fn main() -> std::io::Result<()> {
    let n = 100;
    let f = Arc::new(WaterFactory::new());

    let hs = (0..n * 2).map(|i| {
        let f = f.clone();
        tokio::spawn(async move {
            f.hydrogen(|| bond(&format!("h{i}"))).await;
        })
    });

    let os = (0..n).map(|i| {
        let f = f.clone();
        tokio::spawn(async move {
            f.oxygen(|| bond(&format!("o{i}"))).await;
        })
    });

    future::join_all(Iterator::chain(hs, os)).await;

    Ok(())
}

struct WaterFactory {
    h_sem: Semaphore,
    o_sem: Semaphore,
    barrier: Barrier,
}

impl WaterFactory {
    fn new() -> Self {
        Self {
            o_sem: Semaphore::new(1),
            h_sem: Semaphore::new(2),
            barrier: Barrier::new(3),
        }
    }

    async fn oxygen(&self, bond: impl FnOnce()) {
        let _permit = self.o_sem.acquire().await.unwrap();
        self.barrier.wait().await;
        bond();
    }

    async fn hydrogen(&self, bond: impl FnOnce()) {
        let _permit = self.h_sem.acquire().await.unwrap();
        self.barrier.wait().await;
        bond();
    }
}
