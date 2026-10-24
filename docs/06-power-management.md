# CMC Power and Shutdown Plan

## Intended behavior

**Power on:** BTN1 → PMC reads event → PMC requests R1 power-control stage → Pi boots → CMC reports online. The PMC must be able to initiate Pi power without depending on the Pi already running.

**Graceful power off:** BTN2 → PMC sends shutdown request to CMC → Linux gracefully shuts down → a reliable hardware-level completion/power-ready indication is received → PMC removes Pi power through R1.

**Important:** A UART ACK or the disappearance of a heartbeat does **not** prove Linux has safely shut down. Do not use a fixed timer as the sole basis for cutting power. A suitable power-management circuit or reliable shutdown-ready signaling is required.

## Failure states
- CMC fails to respond: PMC records communication fault; avoid unsafe blind power cut.
- Unexpected CMC disconnect: PMC retains independent monitoring and hardware safety.
- Boot failure: PMC can record boot timeout/fault; recovery strategy TBD.
- PMC restart: output startup defaults must be designed explicitly; do not assume a GPIO's reset state is safe for every relay module.

## Hardware boundaries
R1 is a **proposed** CMC power relay/control channel, not implemented. A correctly rated, isolated driver and proper power management are required. Mains supply design is out of scope for firmware testing. Keep all current tests on low-voltage USB-powered bench hardware.
