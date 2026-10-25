# PMC v0.9 — Configurable Relay GPIO Interface (Simulation)

The board is an eight-channel relay module powered by 5 V with separate
IN1–IN8 pins accepting 3.3 V trigger signals. Trigger polarity can be set
HIGH or LOW on the module; match it in `include/RelayConfig.h`.

## Included
- Eight named logical relay channels with proposed ESP32 GPIO mappings.
- Configurable per-channel `ACTIVE_LOW`/`ACTIVE_HIGH` setting.
- Startup validation of duplicate, unavailable, reserved, I²C and UART pins.
- `RELAY CONFIG`, `RELAY STATUS`, `RELAY TEST Rn ON/OFF` diagnostics.
- R5/R6 ON requests rejected by the generic Relay Manager.
- Existing PMC power state machine, five PCF8574 inputs and CMC bridge retained.

## Important limitations
**This is an input/output-interface configuration model, not a physical relay driver.**
No `pinMode` or `digitalWrite` is used for relay channels. All output states
are requested/simulated; no actual relay state feedback is available.
`RELAY_HARDWARE_OUTPUTS_ENABLED` is false and does not unlock actuation.
The proposed GPIO assignments are not a verified final pinout.

Never interpret an ON state in the console as confirmation that a relay
contact moved. The board's trigger selection, reset/boot behavior, input
current, electrical isolation, and intended switched loads remain to be
verified before any physical-output implementation. Keep tests on a
USB-powered, low-voltage bench setup with no connected loads.
