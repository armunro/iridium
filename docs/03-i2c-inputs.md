# I²C Expanders and Physical Inputs

## Implemented bench wiring

- ESP32 GPIO21 — SDA
- ESP32 GPIO22 — SCL
- ESP32 3V3 — PCF8574 VCC (verify breakout and pull-up voltage)
- ESP32 GND — PCF8574 GND
- Input switch — PCF8574 Pn to GND, active-low

PCF8574 is quasi-bidirectional. Firmware writes `0xFF` to each module to release pins high for input sensing. All five are currently **input-only**.

## Configured addresses (not yet verified on all five boards)

| Module | Proposed address | Function |
|---|---|---|
| IO1 | `0x20` | Main switches |
| IO2 | `0x21` | Main buttons |
| IO3 | `0x22` | Automation buttons |
| IO4 | `0x23` | Additional inputs |
| IO5 | `0x24` | Additional inputs |

Check actual addresses using `I2C SCAN`; set A0/A1/A2 on each board accordingly or edit `EXPANDER_ADDRESSES` in `include/ControlConfig.h`. PCF8574 base address is 0x20; PCF8574A has a different address range and is not interchangeable without configuration changes.

## Firmware mappings

`include/ControlConfig.h` contains:
- `EXPANDER_ADDRESSES` — every module to probe, including unused modules.
- `CONTROL_MAP` — address, pin, button/switch type, human-readable label.
- I²C pins, polling and debounce timings.

Current default mappings:
- 0x20 P0–P5: SW2–SW7
- 0x21 P0–P7: BTN1–BTN8
- 0x22 P0–P3: BTN9–BTN12
- 0x23 and 0x24: no assigned controls; raw unmapped input transitions logged.

There are 18 mapped controls and 22 unmapped pins. This is not a final physical wiring allocation.

## Input behavior
- Active-low logic.
- Poll interval 20 ms; debounce 40 ms.
- Missing devices reported and retried every 2 seconds.
- Reconnected devices resample baseline without fabricating button presses.
- Duplicate pin mappings, duplicate expander addresses and invalid/unregistered addresses are rejected.
- `EVENTS OFF` suppresses logging, not input sampling.
- No mapped button presently energizes a physical output.
