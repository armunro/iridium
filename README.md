# PMC v1.0 — PCF8574 relay driver and event bus

PlatformIO ESP32 Arduino project. Based on v0.9.

**IO1–IO4** (0x20–0x23): switches/buttons. **IO5** (0x24): dedicated eight-bit relay output (P0=R1 through P7=R8). The ESP32 uses GPIO21 SDA and GPIO22 SCL.

The relay driver uses a PCF8574 shadow byte, checks I2C write acknowledgement, rejects ON for all channels except R2/R3, and clears requests when IO5 disappears/reconnects. **Physical outputs default to DISABLED** in `include/RelayConfig.h` until electrical compatibility and startup safety are confirmed. `ACTIVE_LOW` is a configurable proposed trigger mode; the board's global selector must match.

A fixed-size event bus routes physical input events to the state manager, records relay/state events, and supports `BUS STATUS` and `BUS HISTORY`. See `docs/13-i2c-relays-event-bus.md` for constraints and diagnostics.

Build with `pio run -e esp32dev` from this directory. Open serial monitor at 115200 baud and type `HELP`.

This is an engineering development build, **not** a verified safety controller. Do not attach hazardous loads or depend on software for PAL/AUX safety.


## v1.0.1
Fixed Arduino `INPUT` macro collision in `EventBus` (`INPUT_CHANGED`).

## v1.0.2 — Maintained lighting switches
SW2 and SW3 are maintained ON/OFF controls, with startup and reconnection position synchronization, and UNKNOWN state handling on input-expander loss. See `docs/15-maintained-light-switches.md`. Hardware relay outputs remain disabled by default.
