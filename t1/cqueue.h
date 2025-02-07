#pragma once

#include "utils.h"
#include <mutex>
#include <queue>

template <typename T> class ConcurrentQueue {
  std::queue<T> msgs;
  std::mutex mut;
  std::condition_variable cond;

public:
  ConcurrentQueue() : msgs{}, mut{}, cond{} {}

  void enqueue(T msg) {
    {
      std::unique_lock lock{mut};
      msgs.push(msg);
    }

    cond.notify_one();
  }

  std::optional<T> try_dequeue() {
    std::unique_lock lock{mut};

    if (msgs.empty()) {
      return std::nullopt;
    } else {
      // Unfortunately, there's no pop + return in one API call
      // (yet?)
      T msg = msgs.front();
      msgs.pop();

      return msg;
    }
  }

  T dequeue() {
    std::unique_lock lock{mut};

    cond.wait(lock, [this]() { return !msgs.empty(); });
    // Alternatively, this is exactly the same code
    // while (msgs.empty()) {
    //   cond.wait(lock);
    // }

    T msg = msgs.front();
    msgs.pop();

    return msg;
  }
};
