#pragma once

#include <array>
#include <cstdint>

namespace esphome {
namespace balboa_spa {

// State-based toggle delivery. All times are monotonic milliseconds. The UART
// owner calls sent() ONLY after transmitting, and observe() ONLY for a valid
// status packet (never for a periodic publication of cached state).
class ConfirmedCommands {
 public:
  enum Control { JET1, JET2, JET3, JET4, LIGHT, HIGH_RANGE, READY, COUNT };
  using States = std::array<uint8_t, COUNT>;
  static constexpr uint32_t SETTLE_MS = 1500;
  static constexpr uint32_t FRESH_MS = 1000;
  static constexpr uint32_t DEADLINE_MS = 30000;

  void request(Control control, uint8_t target, bool any_on, uint32_t now,
               uint8_t max_attempts = 5) {
    auto &r = requests_[control];
    // Repeated identical requests must not reset the retry/deadline budget.
    if (r.pending && r.target == target && r.any_on == any_on) return;
    r = {true, target, any_on, 0, max_attempts, now};
    // Keep the global in-flight barrier even when reversing a request.
  }

  void observe(const States &states, uint32_t now) {
    states_ = states;
    observed_at_ = now;
    valid_ = true;
    tick(now);
    if (in_flight_ >= 0 && uint32_t(now - sent_at_) >= SETTLE_MS)
      in_flight_ = -1;
    reconcile();
  }

  void tick(uint32_t now) {
    for (int i = 0; i < COUNT; ++i)
      if (requests_[i].pending && uint32_t(now - requests_[i].started) >= DEADLINE_MS)
        fail(i);
  }

  int next(uint32_t now) {
    tick(now);
    if (!valid_ || uint32_t(now - observed_at_) > FRESH_MS || in_flight_ >= 0)
      return -1;
    reconcile();
    for (int n = 0; n < COUNT; ++n) {
      int i = (cursor_ + n) % COUNT;
      auto &r = requests_[i];
      if (!r.pending) continue;
      if (r.attempts >= r.max_attempts) { fail(i); continue; }
      return i;
    }
    return -1;
  }

  void sent(int control, uint32_t now) {
    ++requests_[control].attempts;
    in_flight_ = control;
    sent_at_ = now;
    cursor_ = (control + 1) % COUNT;
  }

  bool pending(Control control) const { return requests_[control].pending; }
  uint8_t target(Control control) const { return requests_[control].target; }
  uint8_t attempts(int control) const { return requests_[control].attempts; }
  uint32_t take_failures() { auto f = failures_; failures_ = 0; return f; }

 private:
  struct Request {
    bool pending{false};
    uint8_t target{0};
    bool any_on{false};
    uint8_t attempts{0};
    uint8_t max_attempts{5};
    uint32_t started{0};
  };
  void reconcile() {
    if (!valid_ || in_flight_ >= 0) return;
    for (int i = 0; i < COUNT; ++i) {
      auto &r = requests_[i];
      if (r.pending && (r.any_on && r.target != 0 ? states_[i] != 0 : states_[i] == r.target))
        r.pending = false;
    }
  }
  void fail(int i) { requests_[i].pending = false; failures_ |= 1u << i; }
  std::array<Request, COUNT> requests_{};
  States states_{};
  bool valid_{false};
  uint32_t observed_at_{0}, sent_at_{0}, failures_{0};
  int in_flight_{-1}, cursor_{0};
};

}  // namespace balboa_spa
}  // namespace esphome
