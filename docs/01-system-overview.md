# System Overview

## Roles

**PMC — Power Management Controller:** Freenove ESP32-WROOM. Reads physical controls through PCF8574 I²C expanders; owns physical outputs, relay supervision, annunciators, local safety state and CMC connectivity. Must remain useful while CMC is down.

**CMC — Central Management Computer:** Raspberry Pi running Linux. Owns automation presets, job/process state, networking operations, HDMI user interface, and higher-level coordination.

## Architecture (target)

```text
Physical switches/buttons/encoder
             |
      PCF8574 inputs
             |
       PMC (ESP32) ---- LED drivers ---- 31 LEDs
             |       \--- relay drivers (future)
       3.3V UART
             |
       CMC (Raspberry Pi) ---- HDMI display
             |
      automation/network services
```

The PMC defines physical control identities in firmware. The CMC decides outcomes for CMC-specific operations. Protected hardware state is independently supervised by the PMC.

## Implementation boundary

**IMPLEMENTED:** ESP32 serial console, five configured input expanders, named input events, diagnostics, simulation commands. v0.6 project also includes a UART bridge design and CMC Python companion; see communications document for verification limits.

**PROPOSED:** Actual relay switching, LED drivers, physical power sequencing, hardware PAL interlocks, automation execution and HDMI interface.
