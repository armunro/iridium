# PAL and AUX Design

## PAL concept
A single authorization channel: CMC requests a time-limited authorization window; PMC independently enforces expiration and hardware prerequisites. Fail locked on missing communications, invalid authorization, or fault.

Physical keyed bypass SW7 is intended to bypass **software permission only**; it must never bypass required physical arming, electrical protection or hardware safety. The bypass mechanism and authorization protocol have not been finalized or implemented.

PAL indicator states: MASTER, BYPASSED, AUTHORIZED, FAULT.

## AUX
- SW4: guarded AUX main.
- SW5: regulated/passthrough source selection.
- SW6: physical AUX ARM.
- BTN7: trigger mode MOM, LATCH, TMR, SD.
- BTN8: FIRE request.
- R5: proposed PAL interlock.
- R6: proposed AUX load trigger.
- AUX input/output and smart AUX interface are TBD.

## Enforcement principles
- PMC is the independent authority for interlocks and authorization expiry.
- Loss of CMC or missing required inputs must not authorize an output.
- Trigger mode/state changes must not cause unintended activation.
- The currently implemented `PAL AUTH`, `PAL LOCK`, `PAL FAULT`, `PAL RESET` are **serial-console simulations only**.

**Status: PROPOSED.** No AUX output hardware, mains switching, or protected-load actuation is implemented or tested.
