#!/usr/bin/env python3
"""CMC bench bridge: receive ESP32 events over a 3.3V UART serial port.
Requires: python3 -m pip install pyserial
No GPIO or relay control. No privileged system operations.
"""
import argparse
import time
import serial

parser = argparse.ArgumentParser()
parser.add_argument('--port', default='/dev/serial0', help='Pi UART device')
parser.add_argument('--baud', type=int, default=115200)
args = parser.parse_args()

with serial.Serial(args.port, args.baud, timeout=0.25) as uart:
    print(f'[CMC] Listening on {args.port} at {args.baud}')
    while True:
        raw = uart.readline()
        if not raw:
            continue
        line = raw.decode('ascii', errors='replace').strip()
        if line == 'PING':
            uart.write(b'PONG\n')
        elif line.startswith('EVENT|'):
            fields = line.split('|')
            if len(fields) == 3:
                _, control, state = fields
                print(f'[CMC] {control}: {state}', flush=True)
                uart.write(b'ACK|EVENT_RECEIVED\n')
            else:
                print('[CMC] Malformed event ignored', flush=True)
        else:
            print(f'[CMC] Unknown: {line}', flush=True)
