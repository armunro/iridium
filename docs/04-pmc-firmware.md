# PMC Firmware Reference

## Build
- VS Code + PlatformIO
- Board: `esp32dev`
- Framework: Arduino
- Monitor: 115200 baud
- I²C: ESP32 GPIO21 SDA, GPIO22 SCL

Open project root and build/upload via PlatformIO. The configuration file is `include/ControlConfig.h`.

## Serial commands

| Command | Function |
|---|---|
| HELP | Show commands |
| PING | Reply PONG |
| STATUS | PMC uptime, simulated PAL/automation, I²C health |
| I2C SCAN | Detect I²C device addresses |
| MAP | Print named control assignments and module availability |
| INPUTS | Print mapped control states |
| EVENTS ON / OFF | Toggle console input event logging |
| HEARTBEAT ON / OFF | Toggle heartbeat console logging |
| PAL AUTH | Simulate a 10-second authorization window |
| PAL LOCK | Set simulated PAL locked |
| PAL FAULT | Set simulated PAL fault |
| PAL RESET | Reset simulated PAL to locked |
| AUTO START / PAUSE / STOP | Change simulated automation state |

**Do not mistake simulation output for actual relay, PAL or automation functionality.**

## Modules
- `src/main.cpp` — serial command handling, simulated state, heartbeat.
- `src/ControlManager.cpp` / `include/ControlManager.h` — PCF8574 probing, polling, debounce, diagnostics.
- `include/ControlConfig.h` — expander and control mappings.
- CMC bridge files (if present in project) — UART communication; consult source before deployment.

## Expected input log
```
[EVENT] SW2 CASE_LIGHTS ON
[EVENT] SW2 CASE_LIGHTS OFF
[INPUT] 0x23/P0 LOW (UNMAPPED)
[INPUT] 0x23/P0 HIGH (UNMAPPED)
```

## Known limitations
- Firmware has not been compiled/hardware-tested as part of this documentation update.
- No LED driver implementation.
- No physical relay actuation.
- No real process automation or HDMI interface.
- On I²C bus contention or electrical failure, diagnostics may report missing devices.
