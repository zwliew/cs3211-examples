#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <random>

struct Message {
  uint32_t type;
  uint32_t sender;
  uint64_t timestamp;
  std::array<uint8_t, 32> data;

  [[nodiscard]] std::string to_string() const {
    return std::format("Message{{type={}, sender={}, timestamp={}, data={}}}",
                       type, sender, timestamp, data);
  }

  static Message generate_message() {
    static std::random_device rd;
    Message msg;
    uint8_t *raw_bytes = reinterpret_cast<uint8_t *>(&msg);
    for (size_t i = 0; i < sizeof(Message); ++i) {
      raw_bytes[i] = static_cast<uint8_t>(rd());
    }
    return msg;
  }
};

struct Logger {
public:
  Logger(const std::string &filename) : os(filename, std::ios::out) {}

  void log_message(const Message &msg) { os << msg.to_string() << '\n'; }

private:
  std::ofstream os;
};

struct Timer {
public:
  Timer(std::string label)
      : label(std::move(label)),
        start(std::chrono::high_resolution_clock::now()) {}

  ~Timer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::format("{} took {} msec\n", label, duration.count());
  }

private:
  std::string label;
  std::chrono::high_resolution_clock::time_point start;
};
