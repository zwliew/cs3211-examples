#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

std::vector<int> data;
std::atomic<int> flag = {0};
int x = 1;

void thread_1() {
  data.push_back(42);
  flag.store(1, std::memory_order::release);
}

void thread_2() {
  // Question part 1 of 2: Suppose we uncomment this...
  // x = 2;

  int expected = 1;
  // memory_order::relaxed is okay because this is an RMW,
  // and RMWs (with any ordering) following a release form a release sequence
  while (
      !flag.compare_exchange_strong(expected, 2, std::memory_order::relaxed)) {
    expected = 1;
  }
}

void thread_3() {
  while (flag.load(std::memory_order::acquire) < 2)
    ;
  // if we read the value 2 from the atomic flag, we see 42 in the vector
  assert(!data.empty() && data[0] == 42); // will never fire

  // Question part 2 of 2: ...will this assertion fire?
  //                       If not, what can we do to make it pass?
  //                       Also, are there data races wrt x?
  // assert(x == 2);
}

int main() {
  std::thread a(thread_1);
  std::thread b(thread_2);
  std::thread c(thread_3);
  a.join();
  b.join();
  c.join();
}
