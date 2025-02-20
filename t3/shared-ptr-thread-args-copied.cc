#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> count(0);

template <typename T> struct SharedPtr4 {
private:
  std::atomic<size_t> *m_count;
  T *m_ptr;

public:
  SharedPtr4(T *ptr) : m_count(new std::atomic<size_t>(1)), m_ptr(ptr) {
    std::cout << "Initialized\n";
  }

  SharedPtr4(const SharedPtr4 &other)
      : m_count(other.m_count), m_ptr(other.m_ptr) {
    std::cout << "Copied\n";
    m_count->fetch_add(1, std::memory_order::relaxed);
  }

  ~SharedPtr4() {
    std::cout << "Destroyed\n";
    size_t old_count = m_count->fetch_sub(1, std::memory_order::acq_rel);
    if (old_count == 1) {
      delete m_ptr;
      delete m_count;
    }
  }

  // shouldn't need to worry about this
  T *get() { return m_ptr; }
  const T *get() const { return m_ptr; }
};

void copy_into_thread(int k,
                      // ptr is copied here (1 of 3)
                      SharedPtr4<int> ptr) {
  if (k == 0)
    return;

  std::thread(
      [](int k,
         // ptr is copied here (2 of 3)
         SharedPtr4<int> ptr) {
        copy_into_thread(k, ptr);
        ++count;
      },
      k - 1,
      // ptr is copied here (3 of 3)
      ptr)
      .detach();
}

// Same as copy_into_thread, but passes `ptr` by reference where possible.
//
// Note: ptr is still copied into the std::thread object, but is passed into the
//       executed function by reference.
void copy_into_thread_by_ref(int k,
                             // Difference 1 of 2
                             const SharedPtr4<int> &ptr) {
  if (k == 0)
    return;

  std::thread(
      [](int k,
         // Difference 2 of 2
         const SharedPtr4<int> &ptr) {
        copy_into_thread_by_ref(k, ptr);
        ++count;
      },
      k - 1,
      // Note:            ptr is still being copied here.
      // Question 1 of 2: If we wanted to, how can we pass this by reference?
      // Question 2 of 2: Is it a good idea to pass it by reference?
      ptr)
      .detach();
}

int main() {
  {
    auto a = SharedPtr4<int>(new int(69));
    // Try it for yourself: Switch between the two calls to see the difference
    //                      in outputs! Can you explain why they are different?
    copy_into_thread(2, a);
    // copy_into_thread_by_ref(2, a);
  }
  while (count != 2)
    ;
}
