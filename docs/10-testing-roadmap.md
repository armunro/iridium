# Testing and Roadmap

## Verified historically by user
- ESP32 PlatformIO firmware uploaded and serial console worked.
- Initial PCF8574 at 0x20 was discovered using `I2C SCAN`.
- Button tests produced distinct input events on multiple PCF8574 pins.

## Not yet verified
- Addresses of all five connected PCF8574 expanders.
- v0.7 firmware build and operation on target hardware.
- PMC ↔ CMC UART bridge end-to-end.
- LED driver selection and behavior.
- Relay power management, PAL interlocks, AUX.
- Automation engine and HDMI UI.

## Bench test checklist
1. Open project in PlatformIO; build firmware.
2. Power ESP32 from USB with only 3.3 V-compatible I²C modules connected.
3. Run `I2C SCAN` and record five actual addresses.
4. Edit `EXPANDER_ADDRESSES` to match, then reflash.
5. Run `STATUS`, `MAP`, `INPUTS`.
6. Connect a low-voltage button from a PCF8574 pin to GND and observe `EVENTS ON` logging.
7. Test unmapped inputs on IO4/IO5.
8. Disconnect one expander; confirm `MISSING`, then reconnect; confirm recovery without false presses.
9. Later, bench-test 3.3 V UART between PMC and CMC, with no physical output actuation.
10. Later, test LED driver separately with a single low-voltage indicator.

## Roadmap
- **v0.7:** five configured input expanders (included).
- **Next:** verify hardware addresses and end-to-end UART message format.
- **Next:** formalize typed event router and stable protocol with sequence IDs.
- **Next:** prototype one dedicated LED driver and `LedConfig.h`.
- **Later:** Pi shutdown-ready detection and safe power-stage design.
- **Later:** CMC automation manager + HDMI UI.
- **Later:** protected hardware supervision, after independent safety review.

## Open decisions
- Actual I²C addresses for IO1–IO5.
- Pi model and serial device/configuration.
- Encoder pin allocation.
- LED driver model, supply, and electrical limits.
- Physical relay driver and safe power sequencing.
- UART framing, retry and versioning.
- Smart AUX connector/protocol and remote trigger.
