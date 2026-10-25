# v1.0: IO5 relay driver and event bus

## I2C ownership
- IO1 0x20, IO2 0x21, IO3 0x22, IO4 0x23: input manager only.
- IO5 0x24: relay manager only; P0..P7 correspond to IN1..IN8.
- SDA GPIO21, SCL GPIO22, common I2C bus.
- Never allow the input manager to probe/write 0x24 as an input.

## Relay safety boundaries
- `include/RelayConfig.h` is the single configuration point.
- The relay board has one global HIGH/LOW trigger selector.
- Default `ACTIVE_LOW` is a **proposal**, not a verified electrical property.
- Hardware outputs are disabled by default. When enabled, only R2 and R3 are permitted to turn ON; R1/R4/R5/R6/R7/R8 cannot be turned ON.
- IO5 writes an inactive byte when first detected; if it disconnects and reconnects, the driver resets desired states OFF.
- A PCF8574 cannot reliably source current in its HIGH state; 3.3V-compatible ESP32 input claims do not establish PCF8574 compatibility. Verify the actual relay input circuit before any hardware-enable change.
- No firmware can guarantee safe outputs while the ESP32 or expander is unpowered/resetting. Use external hardware that ensures the intended default state.
- A successful I2C write does not prove a relay contact moved. No contact feedback is implemented.
- Relay requests made during disconnection are rejected; not queued for later activation.

## Event bus
- Fixed 24-entry queue, 32-entry circular history, 6 subscribers; no dynamic allocations for events.
- `publish()` copies events into history and queues them; `dispatch()` invokes subscribers in loop context.
- Queue overflow increments `dropped`; history still records the event. Overflow is not a reliable durable log.
- Input events route through the bus to `PMCStateManager`; relay requests, changes, faults and state transitions publish events.
- `BUS STATUS` prints queue and drop counters; `BUS HISTORY` prints recent events.

## Serial diagnostics
`I2C SCAN`, `INPUTS`, `MAP`, `RELAY CONFIG`, `RELAY STATUS`, `RELAY TEST R2 ON`, `RELAY TEST R3 OFF`, `RELAY ALL OFF`, `BUS STATUS`, `BUS HISTORY`.

## Limitations / future work
- IO5 detection is checked at startup and on failed writes; a silent disconnection without a subsequent write is not immediately detected.
- Input switch resynchronization after I2C reconnect and independent relay-contact feedback are not implemented.
- CMC power, PAL and AUX relay activation remain disabled pending dedicated safety state machines and electrical verification.
