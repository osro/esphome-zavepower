#include "jet_toggle_component_base.h"

namespace esphome {
namespace balboa_spa {
void JetToggleComponentBase::set_parent(BalboaSpa *parent) { spa = parent; }

void JetToggleComponentBase::update_toggle_state(
    const SpaState *state, int, int &current_state,
    std::function<void(int)> publish_callback) {
  // Keep the requested UI state while awaiting confirmation. On success or
  // failure publish the actual state; never issue commands from cached updates.
  if (!spa->has_status() || spa->control_pending(control_)) return;
  current_state = static_cast<int>(get_jet_state(state));
  publish_callback(current_state);
}

void JetToggleComponentBase::request_state_change(int target, double) {
  bool any_on = state_matches_target(2, 1);
  spa->request_control(control_, target, any_on, max_toggle_attempts);
}
}  // namespace balboa_spa
}  // namespace esphome
