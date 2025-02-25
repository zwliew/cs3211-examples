#include <atomic>
#include <thread>

template <typename T> struct SharedPtr4 {
private:
  std::atomic<size_t> *m_count;
  T *m_ptr;

public:
  SharedPtr4(T *ptr) : m_count(new std::atomic<size_t>(1)), m_ptr(ptr) {}

  SharedPtr4(const SharedPtr4 &other)
      : m_count(other.m_count), m_ptr(other.m_ptr) {
    m_count->fetch_add(1, std::memory_order::relaxed);
  }

  ~SharedPtr4() {
    // New code. Notice the change in memory order, and the extra load.
    size_t old_count = m_count->fetch_sub(1, std::memory_order::release);
    if (old_count == 1) {
      m_count->load(std::memory_order::acquire);
      delete m_ptr;
      delete m_count;
    }

    // Old code. Notice the change in memory order.
    // size_t old_count = m_count->fetch_sub(1, std::memory_order::acq_rel);
    // if (old_count == 1) {
    //   delete m_ptr;
    //   delete m_count;
    // }
  }

  // shouldn't need to worry about this
  T *get() { return m_ptr; }
  const T *get() const { return m_ptr; }
};

void copy_into_thread(int k, SharedPtr4<int> ptr) {
  if (k == 0)
    return;

  std::thread([](int k, SharedPtr4<int> ptr) { copy_into_thread(k, ptr); },
              k - 1, ptr)
      .detach();
}

int main() {
  auto a = SharedPtr4<int>(new int(69));
  copy_into_thread(10, a);
}
