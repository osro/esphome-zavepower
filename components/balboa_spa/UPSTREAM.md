# Balboa component provenance

Based on brianfeucht/esphome-balboa-spa **v2.30**, commit
`2267cf140fc5df793493a2916c92738f047cbc1f`.
The upstream BSD-2-Clause license is preserved in `LICENSE`.

This repository maintains the component so command confirmation can operate at
the actual transmission/status-packet boundary. Local changes:

- `confirmed_commands.h`: bounded desired-state delivery for jets 1–4, light 1,
  temperature range, and Ready/Rest mode.
- `balboaspa.*`: transmit through the controller's clear-to-send slots; feed
  validated status packets into confirmation, including unchanged packets;
  mask the main light's bits independently of the second light.
- Jet fan/switch adapters: use shared retries instead of cached-update counting.
- Light/range switches and Ready/Rest callers: absolute targets; retain requested
  switch state while pending and restore reported state on failure.
- `max_toggle_attempts` accepts 1–5. `discard_updates` is accepted for backwards
  compatibility but has no effect on jets; time and fresh packets govern retries.

Other upstream controls retain their original behaviour. In particular, filter
schedules, filter enable, temperature setpoint, blower, and light 2 are not part
of the confirmation mechanism. Review local changes before replacing this copy
with a newer upstream version.

Run `sh tests/run_native.sh` at repository root for state-machine and UART tests;
run `python tests/check_firmware.py --compile` in an ESPHome environment for the
full device build.
