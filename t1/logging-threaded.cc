#include "cqueue.h"
#include "utils.h"
#include <iostream>
#include <thread>

void run_handler(size_t num_messages, ConcurrentQueue<Message> &log_queue) {
  for (size_t i = 0; i < num_messages; ++i) {
    Message msg = Message::generate_message();
    // Instead of logging directly in the handler thread,
    // we push the message to the logger queue.
    log_queue.enqueue(msg);
    for (size_t j = 0; j < 100; ++j) {
      // Pretend to process the message by busy waiting for 100 iterations.
    }
  }
}

void run_logger(size_t num_messages, ConcurrentQueue<Message> &log_queue) {
  Logger logger("log.txt");
  for (size_t i = 0; i < num_messages; ++i) {
    Message msg = log_queue.dequeue();
    logger.log_message(msg);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <num_messages>\n";
    return 1;
  }
  size_t num_messages = std::stoul(argv[1]);
  {
    Timer overall_timer("Overall");
    ConcurrentQueue<Message> log_queue;
    std::thread logger_thread(run_logger, num_messages, std::ref(log_queue));
    {
      Timer handler_timer("Handler");
      std::thread handler_thread(run_handler, num_messages,
                                 std::ref(log_queue));
      handler_thread.join();
    }
    logger_thread.join();
  }
}
