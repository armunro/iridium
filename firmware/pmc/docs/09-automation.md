# CMC Automation Console

## Proposed UI
Raspberry Pi HDMI console with selectable presets for backups and network tasks. BTN9 opens console; ENC1 rotates through presets, short press selects, long press returns. BTN10 START, BTN11 PAUSE, BTN12 STOP.

## Execution model
CMC owns job execution and truth of job state. Suggested supervisor: dedicated Automation Manager using process supervision (potentially systemd transient services), logs, exit codes and explicit cancellation semantics.

## State machine
IDLE → RUNNING → PAUSED → RUNNING → STOPPING → COMPLETED/FAILED; also RUNNING → COMPLETED/FAILED. Precise transition rules remain to be specified.

Pausing/stopping must be supported by the job type. A file copy or backup must use cooperative cancellation/checkpoints where necessary, not arbitrary process suspension that might corrupt output.

## Indicators
FLED14 console open, FLED15 running, FLED16 paused, FLED17 stopping. PMC displays states reported by CMC; acknowledgements alone do not mean a job actually started or finished.

**Status: PROPOSED.** The ESP32 AUTO commands currently modify simulated states only.
