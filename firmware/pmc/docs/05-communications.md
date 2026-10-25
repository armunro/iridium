# PMC ↔ CMC Communication

## Target physical interface
- Dedicated 3.3 V UART, common ground.
- ESP32 TX GPIO17 → Pi UART RX.
- ESP32 RX GPIO16 ← Pi UART TX.
- Pi UART GPIO assignment/device path must be checked against Pi model and Linux configuration.
- Do not connect 5 V UART levels to either board.

## Division of responsibility
- PMC sends physical input events and reports hardware/health status.
- CMC sends actual Linux/service/automation status.
- PMC renders statuses on annunciators and independently supervises protected outputs.
- CMC is never granted arbitrary unrestricted relay control.

## Illustrative protocol (not a final wire specification)
```
PMC -> CMC: {"type":"input","control":"BTN10","event":"PRESSED"}
CMC -> PMC: {"type":"automation","state":"RUNNING","job":"USB_BACKUP"}
CMC -> PMC: {"type":"indicator","id":"FLED15","state":"ON"}
```
These JSON messages describe **proposed semantics**, not necessarily the exact format used by the included v0.6 UART bridge. Inspect the included bridge source before integration.

## Reliability requirements
- Heartbeats and CMC online/offline state.
- Explicit message version and framing.
- Acknowledgements, sequence IDs, timeout/retry policy.
- Safe behavior on reconnect, duplicate messages and link loss.
- CMC reports authoritative process state; PMC should not infer success from a command acknowledgement.
- Commands affecting protected outputs must be validated by PMC independently.

## Development milestone
Press a PCF8574 button → PMC recognizes mapping → CMC bridge receives event → CMC acknowledges → PMC reports response. No relay output required.
