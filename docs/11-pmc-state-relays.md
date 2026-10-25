# PMC v0.8 — State and Relay Managers

## Implemented (simulation)
- `PMCStateManager`: CMC power OFF, BOOT_REQUESTED, RUNNING, SHUTDOWN_REQUESTED, FAULT.
- 30-second simulated boot/shutdown timeout produces FAULT without cutting power.
- `RelayManager`: eight independent **logical** R1–R8 relay requests, all start OFF.
- PCF8574 events invoke state manager: BTN1 power on; BTN2 request shutdown; BTN3 toggle peripherals R4; SW2 R2; SW3 R3.
- Existing five-expander support, CMC UART bridge and PAL/AUTO simulation retained.

## Serial commands
- `STATE`, `RELAYS` / `RELAY STATUS`
- `POWER ON`, `POWER SHUTDOWN`, `POWER SIM BOOTED`, `POWER SIM HALTED`, `POWER RESET FAULT`
- `RELAY TEST R1 ON`, `RELAY TEST R1 OFF` (replace 1 with 1–8)

## Test sequence
```
STATE
POWER ON
RELAYS
POWER SIM BOOTED
POWER SHUTDOWN
RELAYS
POWER SIM HALTED
RELAYS
```
R1 is logically ON during simulated shutdown until `POWER SIM HALTED`. A mere ACK or timeout never cuts power.

## Safety and limits
**No GPIO or PCF8574 relay outputs are implemented.** Logical ON does not energize anything. `RELAY TEST` is simulated. The design deliberately does not drive mains or protected AUX loads. To add real low-voltage relay control later, first identify the relay module model, coil voltage/current, driver type, polarity, power source, and safe default behavior; do not connect relay coils directly to ESP32 pins. R5/R6 are not automatically actuated by PAL/AUX simulation.

## Known limitation
If a configured expander is missing at startup, physical switches on it have unknown states; they are not automatically asserted. This version should not be used as a safety controller. Firmware has not been compiled or bench-tested on target hardware.
