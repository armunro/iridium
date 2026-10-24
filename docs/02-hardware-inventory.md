# Hardware Inventory and Control Catalog

This is the latest consolidated **design intent**, not a final wiring diagram.

## Ports
- PORT1 — IEC C14 external AC inlet
- PORT2 — Ethernet
- PORT3 — External AUX IN
- PORT4 — Remote trigger, TBD
- PORT5 — PAL data / Smart AUX, TBD
- PORT6 — HDMI display output

## Switches
- SW1 — Main AC power (independent hardware switch)
- SW2 — Case lights
- SW3 — Panel underlay lights
- SW4 — Guarded AUX main
- SW5 — AUX source: regulated / passthrough
- SW6 — AUX arm
- SW7 — Keyed PAL bypass

## Buttons
- BTN1 — CMC power on
- BTN2 — CMC graceful shutdown and power off
- BTN3 — Toggle peripherals
- BTN4 — Toggle Ethernet
- BTN5 — Toggle Wi-Fi
- BTN6 — Toggle SSH
- BTN7 — Cycle AUX trigger: MOM / LATCH / TMR / SD
- BTN8 — FIRE
- BTN9 — Open automation console
- BTN10 — Automation START
- BTN11 — Automation PAUSE
- BTN12 — Automation STOP

## Rotary encoder
- ENC1 — Rotate to navigate/select presets; short press select; long press back. GPIO/pin allocation TBD.

## Controllers
- PMC — Freenove ESP32-WROOM, PlatformIO `esp32dev`
- CMC — Raspberry Pi, model/OS/serial device to verify
- IO1–IO5 — Five PCF8574 8-bit I²C modules, 40 total quasi-bidirectional pins

## Power hardware (proposed)
- PS1 — 65 W AC-to-USB supply (three PD ports, three 3A USB ports)
- REG1 — Adjustable 5–30 V buck/boost regulator
- R1 — CMC power control
- R2 — Case lights
- R3 — Panel underlay lights
- R4 — USB peripherals
- R5 — PAL authorization interlock
- R6 — AUX load trigger
- R7/R8 — Reserved

**Safety:** These relay roles are functional requirements, not permission to directly connect a GPIO to a relay or mains load. Actual hardware design, isolation, protection and power sequencing are not yet validated.
