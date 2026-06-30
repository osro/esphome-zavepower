# Zavepower SmartBox Config Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add an end-user ESPHome example config for running the `balboa_spa` component on the elysics "Pool Sweden SmartBox" (ESP32-S3-WROOM-1), plus discovery firmware to confirm its unknown GPIOs and a status-LED health indicator.

**Architecture:** Pure ESPHome YAML, no C++ changes. Two single-purpose discovery firmware files empirically confirm the Status LED and the spa UART pins on the real board (a GPIO can't hold two roles in one config, so discovery is split by role). The board has only Reset and Prog buttons, so there is no user button to discover. A final sample config wires the confirmed pins, the `balboa_spa` component, and the onboard LED as a `status_led` health indicator. A README section documents the workflow.

**Tech Stack:** ESPHome (esp-idf framework), ESP32-S3-WROOM-1, the local `balboa_spa` external component, USB Serial/JTAG logging.

## Global Constraints

- Target board: `esp32-s3-devkitc-1`, framework `esp-idf` (verbatim from spec §2).
- Logging over USB-C via S3 native USB: `logger: { hardware_uart: USB_SERIAL_JTAG }`.
- Spa UART parameters (verbatim from existing configs): `baud_rate: 115200`, `data_bits: 8`, `parity: NONE`, `stop_bits: 1`, `rx_buffer_size: 512`.
- Known-by-convention pins (excluded from discovery): Prog = `GPIO0`, Reset = `EN`.
- Discovery candidate GPIO pool (safe broken-out S3 pins, excludes GPIO0/19/20/26–37): `1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,21,38,39,40,41,42,43,44,45,46,47,48`.
- **Verification model:** ESPHome configs are not unit-testable; the build is the test. Each task's gate is `esphome config <file>` (validates schema, pins, substitutions) and, where noted, `esphome compile <file>` (full C++ build).
- File naming follows the existing flat repo layout with a `zavepower_` prefix.

---

## Setup (prerequisite for all tasks)

- [ ] **Install ESPHome** (once, if not present)

Run:
```bash
python3 -m pip install --user esphome && esphome version
```
Expected: prints a version like `Version: 2025.x.x`.

- [ ] **Add build/secrets artifacts to .gitignore**

Append these lines to `.gitignore`:
```
# ESPHome build output and local secrets
.build/
/secrets.yaml
```

- [ ] **Commit**
```bash
git add .gitignore
git commit -m "chore: gitignore esphome build output and local secrets"
```

---

## Task 1: Status LED discovery firmware

**Files:**
- Create: `zavepower_led_discovery.yaml`

**Interfaces:**
- Consumes: nothing.
- Produces: a flashable firmware that blinks each candidate GPIO for ~3s and logs `>>> Now blinking GPIOxx`. Operator output: the confirmed Status-LED GPIO number and its polarity (active-high vs active-low).

- [ ] **Step 1: Create the discovery config**

Create `zavepower_led_discovery.yaml`:
```yaml
# Zavepower SmartBox (ESP32-S3-WROOM-1) — Status LED GPIO discovery
#
# 1. Flash over USB-C:   esphome run zavepower_led_discovery.yaml
# 2. Watch the logs:     esphome logs zavepower_led_discovery.yaml
# The firmware blinks each candidate GPIO for ~3s, logging which one is active.
# Watch the board's Status LED for the window where it visibly blinks:
#   - Active-high LED: normally OFF, blinks ON during its window.
#   - Active-low LED:  normally ON,  blinks OFF during its window.
# Record the GPIO printed when the LED reacts, and note the polarity.

esphome:
  name: zavepower-led-discovery
  build_path: .build/zavepower_led_discovery

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: esp-idf

logger:
  level: INFO
  hardware_uart: USB_SERIAL_JTAG

output:
  - { platform: gpio, pin: GPIO1,  id: cand_1 }
  - { platform: gpio, pin: GPIO2,  id: cand_2 }
  - { platform: gpio, pin: GPIO3,  id: cand_3 }
  - { platform: gpio, pin: GPIO4,  id: cand_4 }
  - { platform: gpio, pin: GPIO5,  id: cand_5 }
  - { platform: gpio, pin: GPIO6,  id: cand_6 }
  - { platform: gpio, pin: GPIO7,  id: cand_7 }
  - { platform: gpio, pin: GPIO8,  id: cand_8 }
  - { platform: gpio, pin: GPIO9,  id: cand_9 }
  - { platform: gpio, pin: GPIO10, id: cand_10 }
  - { platform: gpio, pin: GPIO11, id: cand_11 }
  - { platform: gpio, pin: GPIO12, id: cand_12 }
  - { platform: gpio, pin: GPIO13, id: cand_13 }
  - { platform: gpio, pin: GPIO14, id: cand_14 }
  - { platform: gpio, pin: GPIO15, id: cand_15 }
  - { platform: gpio, pin: GPIO16, id: cand_16 }
  - { platform: gpio, pin: GPIO17, id: cand_17 }
  - { platform: gpio, pin: GPIO18, id: cand_18 }
  - { platform: gpio, pin: GPIO21, id: cand_21 }
  - { platform: gpio, pin: GPIO38, id: cand_38 }
  - { platform: gpio, pin: GPIO39, id: cand_39 }
  - { platform: gpio, pin: GPIO40, id: cand_40 }
  - { platform: gpio, pin: GPIO41, id: cand_41 }
  - { platform: gpio, pin: GPIO42, id: cand_42 }
  - { platform: gpio, pin: GPIO43, id: cand_43 }
  - { platform: gpio, pin: GPIO44, id: cand_44 }
  - { platform: gpio, pin: GPIO45, id: cand_45 }
  - { platform: gpio, pin: GPIO46, id: cand_46 }
  - { platform: gpio, pin: GPIO47, id: cand_47 }
  - { platform: gpio, pin: GPIO48, id: cand_48 }

interval:
  - interval: 250ms
    then:
      - lambda: |-
          static int idx = 0;
          static int tick = 0;
          static const int gpios[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,21,38,39,40,41,42,43,44,45,46,47,48};
          output::BinaryOutput *outs[] = {
            id(cand_1), id(cand_2), id(cand_3), id(cand_4), id(cand_5),
            id(cand_6), id(cand_7), id(cand_8), id(cand_9), id(cand_10),
            id(cand_11), id(cand_12), id(cand_13), id(cand_14), id(cand_15),
            id(cand_16), id(cand_17), id(cand_18), id(cand_21), id(cand_38),
            id(cand_39), id(cand_40), id(cand_41), id(cand_42), id(cand_43),
            id(cand_44), id(cand_45), id(cand_46), id(cand_47), id(cand_48)
          };
          const int n = sizeof(gpios) / sizeof(gpios[0]);
          if (tick == 0) {
            ESP_LOGI("led_discovery", ">>> Now blinking GPIO%d for ~3s", gpios[idx]);
          }
          if (tick % 2 == 0) {
            outs[idx]->turn_on();
          } else {
            outs[idx]->turn_off();
          }
          tick++;
          if (tick >= 12) {
            outs[idx]->turn_off();
            tick = 0;
            idx = (idx + 1) % n;
          }
```

- [ ] **Step 2: Validate the config schema**

Run: `esphome config zavepower_led_discovery.yaml`
Expected: ends with `INFO Configuration is valid!` (no pin/schema errors).

- [ ] **Step 3: Compile (verifies the lambda is valid C++)**

Run: `esphome compile zavepower_led_discovery.yaml`
Expected: ends with `Successfully compiled program.` (first esp-idf build downloads the toolchain and is slow).

- [ ] **Step 4: Commit**
```bash
git add zavepower_led_discovery.yaml
git commit -m "feat: add Status LED GPIO discovery firmware for Zavepower SmartBox"
```

---

## Task 2: Spa UART discovery firmware

**Files:**
- Create: `zavepower_uart_discovery.yaml`

**Interfaces:**
- Consumes: the local `balboa_spa` component at `components/`.
- Produces: a flashable firmware that brings up the spa UART on the substituted pins and exposes the `connected` binary sensor. Operator output: confirmed `spa_rx_pin` / `spa_tx_pin` (the pair where valid spa frames arrive and `Connected` turns ON).

- [ ] **Step 1: Create the discovery config**

Create `zavepower_uart_discovery.yaml`:
```yaml
# Zavepower SmartBox (ESP32-S3-WROOM-1) — Spa UART pin discovery
#
# UART pins cannot be swept at runtime, so this is an edit-and-watch loop:
#   1. Set spa_rx_pin / spa_tx_pin below to a candidate pair.
#   2. Flash + watch logs:  esphome run zavepower_uart_discovery.yaml
#   3. Correct pins -> valid frames in the DEBUG log and "Connected" turns ON.
#      Wrong pins -> silence or CRC/garbage. Try the next pair (and swap RX/TX).
# Likely candidate pairs near the U4 transceiver (RX, TX):
#   (44,43)  (43,44)  (18,17)  (17,18)  (16,15)  (15,16)  (5,4)  (4,5)

substitutions:
  spa_rx_pin: GPIO44
  spa_tx_pin: GPIO43

esphome:
  name: zavepower-uart-discovery
  build_path: .build/zavepower_uart_discovery

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: esp-idf

logger:
  level: DEBUG
  hardware_uart: USB_SERIAL_JTAG

external_components:
  - source:
      type: local
      path: components

uart:
  id: spa_uart_bus
  rx_pin: ${spa_rx_pin}
  tx_pin: ${spa_tx_pin}
  data_bits: 8
  parity: NONE
  stop_bits: 1
  baud_rate: 115200
  rx_buffer_size: 512

balboa_spa:
  id: spa
  spa_temp_scale: F

binary_sensor:
  - platform: balboa_spa
    balboa_spa_id: spa
    connected:
      name: "Spa Connected"
```

- [ ] **Step 2: Validate the config schema**

Run: `esphome config zavepower_uart_discovery.yaml`
Expected: `INFO Configuration is valid!` (confirms the local component resolves and the UART/pin schema is accepted).

- [ ] **Step 3: Compile**

Run: `esphome compile zavepower_uart_discovery.yaml`
Expected: `Successfully compiled program.`

- [ ] **Step 4: Commit**
```bash
git add zavepower_uart_discovery.yaml
git commit -m "feat: add spa UART pin discovery firmware for Zavepower SmartBox"
```

---

## Task 3: Final SmartBox sample config

**Files:**
- Create: `zavepower_smartbox.yaml`
- Create: `secrets.yaml` (gitignored — for local validation only, not committed)

**Interfaces:**
- Consumes: the confirmed pins from Tasks 1–2 (filled into the `substitutions` block), and the `balboa_spa` component via the repo's git `external_components` source.
- Produces: the end-user deliverable — a complete, copy-paste config with WiFi/API/OTA/time, the spa UART, a representative set of `balboa_spa` entities, and the onboard LED as a `status_led` health indicator.

Note on placeholder pins: the `substitutions` block ships with plausible concrete defaults (so `esphome config` validates) and clear comments telling the user to replace them with discovery results. These are intentional template defaults, not plan placeholders.

- [ ] **Step 1: Create a local secrets file for validation (not committed)**

Create `secrets.yaml`:
```yaml
wifi_ssid: "validation-only"
wifi_password: "validation-only"
```

- [ ] **Step 2: Create the sample config**

Create `zavepower_smartbox.yaml`:
```yaml
# Zavepower SmartBox example config
# Hardware: elysics "Pool Sweden SmartBox" PCBA (board marking EB00174-B),
#           ESP32-S3-WROOM-1.
#
# Before flashing, replace the pin values in `substitutions` below with the
# values confirmed using the discovery firmware (see README "Zavepower SmartBox").
# Put your real WiFi credentials in secrets.yaml (wifi_ssid / wifi_password).

substitutions:
  device_name: zavepower-spa
  # --- Pins: confirm with the discovery firmware, then replace these defaults ---
  status_led_pin: GPIO15   # from zavepower_led_discovery.yaml
  spa_rx_pin: GPIO44       # from zavepower_uart_discovery.yaml
  spa_tx_pin: GPIO43       # from zavepower_uart_discovery.yaml

esphome:
  name: ${device_name}

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: esp-idf

logger:
  hardware_uart: USB_SERIAL_JTAG

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

api:

ota:
  - platform: esphome

time:
  - platform: homeassistant
    id: homeassistant_time

external_components:
  - source:
      type: git
      url: https://github.com/brianfeucht/esphome-balboa-spa
      ref: main

uart:
  id: spa_uart_bus
  rx_pin: ${spa_rx_pin}
  tx_pin: ${spa_tx_pin}
  data_bits: 8
  parity: NONE
  stop_bits: 1
  baud_rate: 115200
  rx_buffer_size: 512

balboa_spa:
  id: spa
  # Set this to C or F based on the units your spa is configured for.
  spa_temp_scale: F

# Onboard Status LED as a firmware health indicator: solid when healthy,
# blink patterns for WiFi/API problems.
# If discovery showed the LED is active-low, set inverted: true below.
status_led:
  pin:
    number: ${status_led_pin}
    inverted: false

# --- Representative spa entities. The full set of available entities is in
#     test_balboa_spa_component.yaml. ---
climate:
  - platform: balboa_spa
    balboa_spa_id: spa
    name: "Spa Thermostat"
    visual:
      min_temperature: 62 °F
      max_temperature: 105 °F
      temperature_step: 0.5 °F

switch:
  - platform: balboa_spa
    balboa_spa_id: spa
    jet1:
      name: "Jet 1"
    light:
      name: "Lights"

binary_sensor:
  - platform: balboa_spa
    balboa_spa_id: spa
    connected:
      name: "Spa Connected"
    heatstate:
      name: "Heating"

text_sensor:
  - platform: balboa_spa
    balboa_spa_id: spa
    spa_time:
      name: "Spa Time"
    fault_message:
      name: "Fault Message"

button:
  - platform: balboa_spa
    balboa_spa_id: spa
    sync_time:
      name: "Sync Spa Time"

# --- Alternative LED behaviors (uncomment ONE instead of status_led above) ---
#
# Spa-state indicator (LED follows the heater):
# output:
#   - platform: gpio
#     pin:
#       number: ${status_led_pin}
#       inverted: false
#     id: led_output
# # ...drive id(led_output) from a balboa_spa heatstate binary_sensor automation.
#
# Plain controllable light exposed to Home Assistant:
# light:
#   - platform: binary
#     name: "Status LED"
#     output: led_output
```

- [ ] **Step 3: Validate the config schema**

Run: `esphome config zavepower_smartbox.yaml`
Expected: `INFO Configuration is valid!` (fetches the git component; requires network and the local `secrets.yaml`).

- [ ] **Step 4: Commit (config only — secrets.yaml is gitignored)**
```bash
git add zavepower_smartbox.yaml
git commit -m "feat: add Zavepower SmartBox Balboa Spa example config with status LED"
```

---

## Task 4: README documentation

**Files:**
- Modify: `README.md`

**Interfaces:**
- Consumes: the four new YAML files.
- Produces: a user-facing "Zavepower SmartBox" section covering the board, the discovery workflow, the pinout table, and flashing.

- [ ] **Step 1: Add a Zavepower SmartBox section to README.md**

Add this section to `README.md` (after the "Sample Config" section):
```markdown
## Zavepower SmartBox (ESP32-S3-WROOM-1)

This repo includes an example config for the deprecated Zavepower Energy
Optimizer / elysics "Pool Sweden SmartBox" board (marking `EB00174-B`), built
around an ESP32-S3-WROOM-1. It runs the `balboa_spa` component and drives the
board's onboard Status LED as a firmware health indicator.

### Confirming the GPIO pinout

Most of the board's GPIO routing isn't documented, so confirm the pins on your
own board with the included discovery firmware (flash over USB-C, then watch the
logs). Run each with `esphome run <file>` and `esphome logs <file>`:

| File | Confirms |
|---|---|
| `zavepower_led_discovery.yaml` | Status LED GPIO (and active-high/low polarity) |
| `zavepower_uart_discovery.yaml` | Spa UART RX/TX pins (edit-and-watch loop) |

Known by convention: Prog button = `GPIO0`, Reset = `EN`, USB-C = native USB.

Record your results here:

| Signal | GPIO | Notes |
|---|---|---|
| Status LED | _GPIO?_ | active-high / active-low |
| Spa UART RX | _GPIO?_ | |
| Spa UART TX | _GPIO?_ | |

### Using the config

1. Put your WiFi credentials in `secrets.yaml` (`wifi_ssid`, `wifi_password`).
2. In `zavepower_smartbox.yaml`, set the pin values in `substitutions` to the
   values you confirmed above. If the LED is active-low, set the `status_led`
   `inverted: true`.
3. Flash over USB-C: `esphome run zavepower_smartbox.yaml`.

The config ships with a representative set of spa entities; see
`test_balboa_spa_component.yaml` for the full list you can add.
```

- [ ] **Step 2: Commit**
```bash
git add README.md
git commit -m "docs: document Zavepower SmartBox board and discovery workflow"
```

---

## Self-Review

**Spec coverage:**
- Deliverable: example YAML config → Task 3. ✅
- Discovery firmware (LED / UART) → Tasks 1, 2. ✅ (Split into two files; documented why in Architecture.)
- esp-idf framework + USB logging → Global Constraints, all tasks. ✅
- status_led health indicator + commented spa-state/light alternatives → Task 3. ✅
- README note → Task 4. ✅
- Candidate GPIO pool / convention pins → Global Constraints + Tasks 1–2. ✅
- Out of scope (no C++ changes, CI compile-test deferred) → honored; no task touches `components/` or CI. ✅

**Placeholder scan:** The only `<...>`-style markers are the intentional README result-table blanks (`_GPIO?_`) and template defaults in Task 3, both explicitly called out as fill-in-by-user, not plan gaps. All YAML is complete and concrete. ✅

**Type/name consistency:** `balboa_spa` id is `spa` and uart id is `spa_uart_bus` consistently across Tasks 2 and 3; substitution names `spa_rx_pin`/`spa_tx_pin`/`status_led_pin` match between the config and README; output id `led_output` is consistent within the Task 3 commented alternatives. ✅
