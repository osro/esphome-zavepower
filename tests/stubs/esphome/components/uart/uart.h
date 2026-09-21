#pragma once
#include <cstdint>
#include <deque>
#include <vector>
namespace esphome { namespace uart {
class UARTDevice {
 public:
  std::deque<uint8_t> rx;
  std::vector<uint8_t> tx;
  size_t available() const { return rx.size(); }
  bool read_byte(uint8_t *v) {
    if (rx.empty()) return false;
    *v = rx.front(); rx.pop_front(); return true;
  }
  void write(uint8_t v) { tx.push_back(v); }
  void flush() {}
};
}}
