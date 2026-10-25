# v1.0.2 — Maintained lighting switches

SW2 (`0x20/P0`) requests R2 (case lights), and SW3 (`0x20/P1`) requests R3 (panel lights). Each switch is active-low: closed to GND means ON. The PCF8574 inputs are 3.3 V; do not connect mains to input pins.

- Switch positions are sampled and published during startup and after input expander reconnection.
- Debounced position changes publish `INPUT_CHANGED` events and are routed through `PMCStateManager` to the relay manager.
- On input expander read failure, the switch state becomes UNKNOWN and OFF is requested. Physical OFF cannot be guaranteed if the output expander or relay hardware is also unavailable.
- R2/R3 ON requests remain blocked while `RELAY_HARDWARE_OUTPUTS_ENABLED` is false.
- `INPUTS`, `STATE`, `BUS HISTORY`, and `RELAY STATUS` help inspect behavior.
- Switch behavior constants are in `include/ControlConfig.h`. The maintained switch behavior is implemented for SW2/SW3; MOMENTARY is reserved for future use and does not drive these relays.

**Hardware caution:** PCF8574 cannot strongly source HIGH; validate relay input electrical compatibility and reset defaults before enabling outputs.
