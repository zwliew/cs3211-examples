#include "utils.h"
#include <iostream>
#include <thread>

void run_handler(size_t num_messages) {
  Logger logger("log.txt");
  for (size_t i = 0; i < num_messages; ++i) {
    Message msg = Message::generate_message();
    logger.log_message(msg);
    for (size_t j = 0; j < 100; ++j) {
      // Pretend to process the message by busy waiting for 100 iterations
    }
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
    std::thread handler_thread(run_handler, num_messages);
    handler_thread.join();
  }
}
