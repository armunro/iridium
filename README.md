# PMC v0.6 — CMC UART Bench Link

This extends v0.5 with a **report-only** serial bridge to the Raspberry Pi. No relays, Pi power switching, PAL hardware, or AUX outputs are enabled.

## PMC
Open in PlatformIO, upload to ESP32. USB serial monitor remains at 115200 baud. The PCF8574 on 0x20 remains on SDA=GPIO21/SCL=GPIO22. Edit `include/ControlConfig.h` for input mappings.

## CMC
Run `python3 -m pip install pyserial`, then `python3 cmc/bridge.py --port /dev/serial0` on the Raspberry Pi. Ensure the selected UART is enabled and is not being used as a Linux serial console. Device name varies by Pi model and configuration.

## Link
Use a **3.3V TTL UART** interface only, with a common ground and compatible logic levels. ESP32 GPIO17 (TX) goes to Pi UART RX, and ESP32 GPIO16 (RX) goes to Pi UART TX. Do not connect either controller to RS-232 voltage levels or 5V serial outputs. For initial testing, leave all mains circuits and relay loads disconnected.

## Protocol v0.6 (newline-delimited ASCII)
PMC -> CMC: `PING`, `EVENT|SW2 CASE_LIGHTS|ON`, `EVENT|BTN1 CMC_POWER_ON|PRESSED`
CMC -> PMC: `PONG`, `ACK|EVENT_RECEIVED`, optional `STATE|...` (displayed only)

PMC pings every 2s and marks CMC offline after 6s without PONG/ACK/STATE. Events are transmitted whether or not event logging is enabled. `EVENTS OFF` suppresses local USB logging only.

## Testing
1. On PMC USB monitor: `I2C SCAN`, `MAP`, `CMC STATUS`.
2. Start CMC bridge. After a few seconds, PMC should report `[CMC] ONLINE`.
3. Press the P0 button. CMC console should print the event; PMC USB monitor should show an ACK.
4. Stop CMC bridge; PMC should show OFFLINE after timeout.

## Limits
The v0.6 protocol is a **prototype**: it has no sequence IDs, authentication, retries, or command execution. It must not be used for protected hardware operations. Pi power-on will eventually be handled locally by PMC hardware; this prototype only sends BTN1 events for observation. The event manager still does not actuate relays.

## v0.7: Five PCF8574 input expanders

The firmware registers **five** devices using `EXPANDER_ADDRESSES` in
`include/ControlConfig.h` (defaults: `0x20`, `0x21`, `0x22`, `0x23`, `0x24`).
Confirm actual addresses with `I2C SCAN`; adjust the array if necessary.

- `MAP` displays mapped controls; `STATUS` reports all five modules.
- `INPUTS` displays mapped inputs. Unmapped pins on IO4/IO5 generate
  `[INPUT] 0x23/P0 LOW (UNMAPPED)` etc. when `EVENTS ON`.
- To assign a control to IO4/IO5, change its address/pin in `CONTROL_MAP`.
- The existing CMC UART bridge and commands remain unchanged.
- All five modules are treated as **input-only**. The PCF8574 initializes
  pins high to enable input sensing. Do not use these pins to drive relays
  or LEDs with this firmware.
- Missing devices are reported and automatically retried; the code does
  not infer addresses or assume the proposed defaults match physical boards.
