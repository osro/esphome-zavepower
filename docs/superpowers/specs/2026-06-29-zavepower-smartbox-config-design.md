# Zavepower SmartBox example config — design

**Date:** 2026-06-29
**Status:** Approved (pending spec review)

## Goal

Add support for running the existing `balboa_spa` ESPHome external component on
the deprecated Zavepower Energy Optimizer hardware — the elysics "Pool Sweden
SmartBox PCBA" (board marking `EB00174-B`), built around an
**ESP32-S3-WROOM-1** module. The deliverable is an end-user example YAML config
pre-wired for this board, including driving its onboard Status LED as a health
indicator.

Balboa remains the controlled spa unit; this work only adds a new hardware
target (the SmartBox) as the ESPHome host, alongside the existing
lolin_s2_mini-based test configs.

## Board facts (from photo inspection)

The board exposes:

- ESP32-S3-WROOM-1 module
- `Reset` button and `Prog` button (SW3) — the only two buttons on the board
- a `Status` LED (the `Status` silkscreen labels the LED, not a button)
- USB-C connector (native USB)
- 4-pin JST connector (orange/red/black/white cable) — the link to the spa
- An 8-pin IC `U4` with `Rx`/`Tx` silkscreen near the JST — a UART
  level-shifter/transceiver between the ESP and the spa connector
- On-board buck converter (elysics-branded power module)

### Pin confidence

| Signal | Value | Confidence | Notes |
|---|---|---|---|
| Prog button (SW3) | `GPIO0` | High | Standard boot/strapping pin convention |
| Reset button | `EN` | High | Chip reset, not a GPIO |
| USB-C | native USB CDC (`GPIO19/20`) | High | S3 native USB |
| Status LED | unknown | None | Custom routing — determined by discovery |
| Spa UART TX/RX | unknown | None | Routed via `U4`; determined by discovery |

GPIO assignments cannot be read reliably from a single top-side photo (traces run
on inner layers / the back side). The unknown pins are therefore confirmed
empirically with discovery firmware before being baked into the final config.

## Deliverables

1. **`zavepower_led_discovery.yaml`** — diagnostic firmware to confirm the
   Status LED GPIO (and its polarity).
2. **`zavepower_uart_discovery.yaml`** — diagnostic firmware to confirm the spa
   UART RX/TX pins.
3. **`zavepower_smartbox.yaml`** — the end-user sample config (the primary
   deliverable), pre-wired with confirmed pins + status LED.
4. A short README note documenting the board, the (filled-in) pinout, and how to
   flash over USB-C.

Discovery is split into two single-purpose files because a single GPIO cannot
be declared in two roles (output vs input) within one ESPHome config.

No C++ component changes. This is pure YAML plus the README note.

## Framework & platform

- `esp32: board: esp32-s3-devkitc-1`, framework **`esp-idf`** — matches the
  README sample config and is the modern default for the S3.
- Logging over USB-C via the S3 native USB Serial/JTAG, so discovery output is
  visible with just a USB cable (no WiFi required).

## Phase 1 — Discovery firmware

The discovery firmware runs entirely off USB and prints to the serial log. It
resolves two unknowns, in two files:

- **Status LED GPIO** (`zavepower_led_discovery.yaml`). On boot, an interval
  routine blinks each candidate GPIO for ~3 seconds, logging `Now blinking
  GPIOxx`. The operator watches for the window where the LED reacts; that GPIO
  (and whether it is active-high or active-low) is the answer.
- **Spa UART TX/RX** (`zavepower_uart_discovery.yaml`). Candidates are narrowed
  around the `U4` transceiver, then
  confirmed by watching the `balboa_spa` frame log: correct RX/TX yields valid
  spa frames, wrong pins yield silence or garbage. UART pins cannot be swept at
  runtime, so this is a short edit-and-watch loop over a small candidate list.

**Candidate GPIO pool:** the broken-out, safe ESP32-S3 pins, excluding:

- `GPIO0` (Prog / strapping)
- `GPIO19`, `GPIO20` (native USB)
- `GPIO26`–`GPIO37` (SPI flash / PSRAM)

Known by convention and excluded from discovery: **Prog = GPIO0**,
**Reset = EN**.

## Phase 2 — Final sample config

`zavepower_smartbox.yaml` is a complete, copy-paste config:

- `substitutions` for device name and WiFi credentials
- `wifi`, `api`, `ota`, `logger` (USB CDC), `time` (Home Assistant platform)
- `external_components` pointing at this repository
- `uart` with the confirmed pins, `115200` baud, `8N1`, `rx_buffer_size: 512`
- `balboa_spa` with `spa_temp_scale`, plus the optional spa sub-components users
  typically want
- The onboard LED wired as a **`status_led`** on the confirmed GPIO (inverted if
  active-low), so it reflects firmware health automatically (solid when
  connected; blink patterns for WiFi/API problems)
- Commented-out **spa-state** and **plain controllable light** alternatives for
  users who prefer those LED behaviors

## Out of scope

- No changes to the C++ `balboa_spa` component.
- CI compile-testing of the new configs is a possible follow-up, not part of this
  work. (The final config references WiFi secrets and won't compile in CI as-is;
  a dedicated CI compile stub would be a separate, optional task.)

## Success criteria

- The discovery firmware flashes over USB-C and prints log output that lets the
  operator identify the Status LED GPIO and the spa UART pins.
- The final sample config, with confirmed pins filled in, compiles, connects to
  WiFi/Home Assistant, drives the Status LED as a health indicator, and exchanges
  valid frames with the spa over UART.
