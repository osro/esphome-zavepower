#include "components/balboa_spa/balboaspa.h"
#include <cassert>
#include <iostream>
using namespace esphome;
using namespace esphome::balboa_spa;

// Frames go through the real CRC/parser/CTS/transmitter, only UART and clock
// are replaced. CRC is the protocol's CRC-8/SMBUS with initial/final XOR 2.
std::vector<uint8_t> frame(std::vector<uint8_t> body) {
  body.insert(body.begin(), uint8_t(body.size() + 2));
  uint8_t crc = 2;
  for (auto b : body) {
    crc ^= b;
    for (int i = 0; i < 8; ++i) crc = (crc & 128) ? (crc << 1) ^ 7 : crc << 1;
  }
  body.push_back(crc ^ 2);
  body.insert(body.begin(), 0x7e);
  body.push_back(0x7e);
  return body;
}
void receive(BalboaSpa &spa, const std::vector<uint8_t> &bytes, uint32_t now) {
  test_now = now;
  spa.rx.insert(spa.rx.end(), bytes.begin(), bytes.end());
  spa.update();
}
std::vector<uint8_t> status(bool high, uint8_t lights = 0) {
  std::vector<uint8_t> body(29, 0);
  body[0] = 0xff; body[1] = 0xaf; body[2] = 0x13;
  body[5] = 60; body[8] = 1; // water 30 C, rest mode
  body[13] = high ? 4 : 0;
  body[17] = lights;
  body[23] = 70; // setpoint 35 C
  return frame(body);
}
void cts(BalboaSpa &spa, uint32_t now) {
  spa.tx.clear();
  receive(spa, frame({10, 0xbf, 6}), now);
}
bool toggle(const BalboaSpa &spa, uint8_t command) {
  return spa.tx.size() == 9 && spa.tx[4] == 0x11 && spa.tx[5] == command;
}
int main() {
  BalboaSpa spa;
  spa.setup(); spa.set_spa_temp_scale(TEMP_SCALE::C);
  receive(spa, frame({0xfe, 0xbf, 2, 10}), 0);
  receive(spa, status(false), 100);
  spa.set_highrange(true);
  cts(spa, 100);
  assert(toggle(spa, 0x50));
  // Valid but unchanged status must count as fresh feedback for retries.
  receive(spa, status(false), 1700);
  cts(spa, 1700);
  assert(toggle(spa, 0x50));
  receive(spa, status(true), 3300);
  cts(spa, 3300);
  assert(!toggle(spa, 0x50));
  // Two different controls must not overwrite one another before CTS.
  spa.set_highrange(false);
  spa.toggle_heat();
  cts(spa, 3400);
  assert(toggle(spa, 0x50) || toggle(spa, 0x51));
  uint8_t first = spa.tx[5];
  receive(spa, status(first != 0x50), 5000);
  cts(spa, 5000);
  assert(toggle(spa, first == 0x50 ? 0x51 : 0x50));

  // Light 2 bits must not hide light 1 and provoke repeated toggle retries.
  BalboaSpa lights;
  lights.setup(); lights.set_spa_temp_scale(TEMP_SCALE::C);
  receive(lights, frame({0xfe, 0xbf, 2, 10}), 0);
  receive(lights, status(false, 0x0c), 100);
  lights.request_control(ConfirmedCommands::LIGHT, 1);
  cts(lights, 100);
  assert(toggle(lights, 0x11));
  receive(lights, status(false, 0x0f), 1700);
  cts(lights, 1700);
  assert(!toggle(lights, 0x11));
  assert(lights.get_current_state()->light);

  // Neither corrupt frames nor periodic updates are acknowledgements.
  lights.request_control(ConfirmedCommands::LIGHT, 0);
  cts(lights, 1800);
  assert(toggle(lights, 0x11));
  auto corrupted = status(false, 0x0f);
  corrupted[corrupted.size() - 2] ^= 1;
  receive(lights, corrupted, 3400);
  cts(lights, 3400);
  assert(!toggle(lights, 0x11));
  assert(lights.control_pending(ConfirmedCommands::LIGHT));
  // A CRC-valid short status must not be accepted as an all-OFF confirmation.
  receive(lights, frame({0xff, 0xaf, 0x13}), 3500);
  cts(lights, 3500);
  assert(!toggle(lights, 0x11));
  assert(lights.control_pending(ConfirmedCommands::LIGHT));
  receive(lights, status(false, 0x0c), 3600);
  cts(lights, 3600);
  assert(!toggle(lights, 0x11));
  assert(!lights.control_pending(ConfirmedCommands::LIGHT));

  // Deadline failures reach diagnostics and discard intent before reconnect.
  lights.set_ready(true);
  assert(lights.commands_pending());
  test_now = 33600;
  lights.update();
  assert(!lights.commands_pending());
  assert(lights.command_failed());
  receive(lights, frame({0xfe, 0xbf, 2, 10}), 33700);
  receive(lights, status(false), 33800);
  cts(lights, 33800);
  assert(!toggle(lights, 0x51));
  lights.set_ready(true);
  assert(!lights.command_failed());
  cts(lights, 33900);
  assert(toggle(lights, 0x51));
  std::cout << "UART confirmed command scenarios passed\n";
}
