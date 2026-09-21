#pragma once
#include <cstdint>
#include <cmath>
#include <functional>
#include <vector>
#define PROGMEM
namespace esphome {
inline uint32_t test_now = 0;
inline uint32_t millis() { return test_now; }
inline void yield() {}
namespace setup_priority { constexpr float LATE = -100; }
class PollingComponent {
 public:
  explicit PollingComponent(float) {}
  virtual void setup() {}
  virtual void update() {}
  virtual float get_setup_priority() const { return 0; }
  void status_set_error(const char *) { error_ = true; }
  void status_clear_error() { error_ = false; }
  bool status_has_error() const { return error_; }
  void status_set_warning(const char *) {}
  void status_clear_warning() {}
 private:
  bool error_ = false;
};
}
