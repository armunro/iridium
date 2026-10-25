# LED and Annunciator Plan

## Ownership
The PMC physically drives all indicators and owns the LED state/pattern manager. The CMC supplies authoritative **software** states such as network service status and automation progress.

## Hardware proposal
Two TLC5947 24-channel constant-current LED driver boards → 48 channels for 31 planned LEDs, leaving 17 spare. TLC5947 uses a serial-style interface rather than I²C. Confirm electrical characteristics, LED forward voltage, brightness, module compatibility, and wiring before purchase/integration. The five PCF8574 boards remain dedicated to inputs.

## Main annunciators (10)
- ALED1 — Main AC
- ALED2 — PMC power
- ALED3 — CMC power
- ALED4 — AUX master
- ALED5 — Network master
- ALED6 — USB master
- ALED7 — Display master
- ALED8 — CMC activity
- ALED9 — AUX armed
- ALED10 — AUX load active

## PAL annunciators (4)
- PLED1 — PAL MASTER (green)
- PLED2 — PAL BYPASSED (amber)
- PLED3 — PAL AUTHORIZED (green)
- PLED4 — PAL FAULT (red)

## Function LEDs (17)
- FLED1 — AUX source regulated
- FLED2 — AUX source passthrough
- FLED3 — AUX armed
- FLED4 — PAL bypass active
- FLED5 — Peripherals enabled
- FLED6 — Ethernet enabled
- FLED7 — Wi-Fi enabled
- FLED8 — SSH enabled
- FLED9 — Trigger momentary
- FLED10 — Trigger latching
- FLED11 — Trigger timed
- FLED12 — FIRE ready
- FLED13 — Trigger SD
- FLED14 — Automation console open (blue)
- FLED15 — Automation running (green)
- FLED16 — Automation paused (amber)
- FLED17 — Automation stopping (red)

## Firmware proposal
`LedConfig.h` maps logical IDs to physical channels; `LedManager` handles steady on/off, blinking, faults, brightness and startup test patterns.

Proposed diagnostics: `LED TEST`, `LED ON`, `LED OFF`, `LED STATUS`.

## State authority
- PMC: local power, PAL, AUX, hardware faults.
- CMC: automation state, Wi-Fi, SSH, network service state.
- On loss of CMC heartbeat, dependent LEDs should display an unknown/offline state, not stale success.
- For AC status, use a properly isolated sensing mechanism; do not wire mains directly to a microcontroller.

**Status: PROPOSED; no LED driver code or verified pin allocation yet.**
