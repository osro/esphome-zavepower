#include "components/balboa_spa/confirmed_commands.h"
#include <cassert>
#include <iostream>
using esphome::balboa_spa::ConfirmedCommands;

int main() {
  using C = ConfirmedCommands;
  C c;
  C::States state{};
  // No transmission based on startup defaults, nor old feedback.
  c.request(C::JET1, 2, false, 0);
  assert(c.next(0) == -1);
  c.observe(state, 100);
  assert(c.next(1200) == -1);
  c.observe(state, 1300);
  assert(c.next(1300) == C::JET1);
  c.sent(C::JET1, 1300);
  // A dropped command needs fresh feedback AFTER the settling interval.
  c.observe(state, 1400);
  assert(c.next(2800) == -1);
  c.observe(state, 2900);
  assert(c.next(2900) == C::JET1);
  c.sent(C::JET1, 2900);
  state[C::JET1] = 2;
  c.observe(state, 3000);
  assert(c.next(3000) == -1);
  c.observe(state, 4500);
  assert(!c.pending(C::JET1));
  assert(c.next(4500) == -1);

  // Opposite click while a toggle is in flight must not cancel against old state.
  c.request(C::JET1, 0, false, 4600);
  assert(c.next(4600) == C::JET1);
  c.sent(C::JET1, 4600);
  c.request(C::JET1, 2, false, 4700);
  assert(c.next(4700) == -1);
  state[C::JET1] = 0;
  c.observe(state, 6200);
  assert(c.next(6200) == C::JET1);
  c.sent(C::JET1, 6200);
  state[C::JET1] = 2;
  c.observe(state, 7800);
  assert(!c.pending(C::JET1));

  // Multiple controls survive together; only one toggle is in flight.
  c.request(C::LIGHT, 1, false, 7900);
  c.request(C::HIGH_RANGE, 1, false, 7900);
  int first = c.next(7900);
  assert(first == C::LIGHT || first == C::HIGH_RANGE);
  c.sent(first, 7900);
  assert(c.next(7900) == -1);
  state[first] = 1;
  c.observe(state, 9500);
  int second = c.next(9500);
  assert(second != first && second >= 0);
  c.sent(second, 9500);
  state[second] = 1;
  c.observe(state, 11100);
  assert(c.next(11100) == -1);

  // A switch accepts a single-speed pump reporting HIGH as ON.
  c.request(C::JET1, 1, true, 11200);
  assert(c.next(11200) == -1);
  assert(!c.pending(C::JET1));

  // Bounded attempts when fresh feedback consistently rejects the target.
  C failing;
  failing.request(C::READY, 1, false, 0);
  for (uint32_t t = 0; t < 8000; t += 1600) {
    failing.observe(C::States{}, t);
    assert(failing.next(t) == C::READY);
    failing.sent(C::READY, t);
  }
  failing.observe(C::States{}, 8000);
  assert(failing.next(8000) == -1);
  assert(failing.take_failures() == (1u << C::READY));
  assert(!failing.pending(C::READY));
  assert(failing.take_failures() == 0);

  // Disconnection expires intent, so reconnecting cannot replay old clicks.
  C offline;
  offline.request(C::LIGHT, 1, false, 0);
  offline.tick(30000);
  offline.observe(C::States{}, 31000);
  assert(offline.next(31000) == -1);
  assert(offline.take_failures() == (1u << C::LIGHT));

  // Repeated identical clicks cannot keep a lost request alive indefinitely.
  C repeated;
  repeated.request(C::READY, 1, false, 0);
  repeated.request(C::READY, 1, false, 29000);
  repeated.tick(30000);
  assert(!repeated.pending(C::READY));
  assert(repeated.take_failures() == (1u << C::READY));

  // Reversing an unsent command back to the reported state sends nothing.
  C cancelled;
  cancelled.observe(C::States{}, 100);
  cancelled.request(C::JET1, 2, false, 100);
  cancelled.request(C::JET1, 0, false, 110);
  assert(cancelled.next(110) == -1);
  assert(!cancelled.pending(C::JET1));

  // Clock rollover does not bypass the settling barrier.
  C wrap;
  wrap.observe(C::States{}, 0xFFFFFF00u);
  wrap.request(C::LIGHT, 1, false, 0xFFFFFF00u);
  wrap.sent(C::LIGHT, 0xFFFFFF00u);
  wrap.observe(C::States{}, 100);
  assert(wrap.next(100) == -1);
  wrap.observe(C::States{}, 1400);
  assert(wrap.next(1400) == C::LIGHT);
  std::cout << "Confirmed command scenarios passed\n";
}
