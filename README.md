# nRF52840 BT Spinner Controller

BLE HID gamepad firmware for a **nice!nano V2** and **Bourns ENA1J-B28-L00064L** optical encoder spinner.

## Features (planned / in progress)

- BLE HID gamepad (Windows 10+); no Bluetooth Classic on nRF52840
- Rotary encoder → right analog stick **X**
- Three buttons (expandable)
- Four build flavors: **Spinner (Red|Blue|Green|Yellow)** for pairing UI
- PlatformIO + Adafruit nRF52 Arduino (Bluefruit)

## Hardware notes

- Encoder: **5.0 V** supply (boost from LiPo); **level-shift** A/B to 3.3 V before MCU pins
- Do not power the encoder from 3.3 V or the LiPo directly
- Pin placeholders live in `include/hardware/pins.h` — update for your proto layout

## Prerequisites

- [PlatformIO](https://platformio.org/)
- USB cable; **double-tap reset** on nice!nano for serial DFU upload
- Optional: [adafruit-nrfutil](https://github.com/adafruit/Adafruit_nRF52_nrfutil) if upload fails from PIO alone

## Build & flash

```bash
# Default environment: spinner_red
pio run

# Build a specific color
pio run -e spinner_blue

# Upload (put board in bootloader: double-tap RST)
pio run -e spinner_red -t upload

# Serial monitor
pio device monitor
```

Flash the environment that matches each physical unit (Red/Blue/Green/Yellow).

Display names are defined in `include/config.h` from `DEVICE_VARIANT_ID` (avoids fragile quoted `-D` strings on Windows).

## Project layout

```
platformio.ini       # Shared config + 4 color environments
boards/nicenano/     # nice!nano V2 board definition (variant from community BSP)
include/
  config.h           # Build-time names and encoder constants
  hardware/pins.h    # Pin map (edit for your wiring)
  ble_gamepad.h
  encoder_qdec.h
  buttons.h
src/
  main.cpp
  ble_gamepad.cpp    # Bluefruit BLE HID + advertising name
  encoder_qdec.cpp   # nRF52840 hardware QDEC (Adafruit RotaryEncoder)
  buttons.cpp
Docs/                # Datasheets (encoder, etc.)
```

## Next implementation steps

1. Finalize `include/hardware/pins.h` for QDEC-capable GPIO
2. Tune `ENCODER_INVERT_DIRECTION` / `ENCODER_QDEC_SAMPLEPER` in `config.h` if needed
3. Tune `ENCODER_COUNTS_FOR_FULL_DEFLECTION` for game feel
4. Custom HID descriptor (optional): hide unused sticks if Windows mapping needs it
5. SAADC read for potentiometer on right stick **Y**

## HID layout

Custom descriptor in `include/hid_gamepad_descriptor.h`:

| Field | HID usage | Role |
|-------|-----------|------|
| `z` | Desktop Z | Encoder (spinner) |
| `rz` | Desktop Rz | Potentiometer (paddle) |
| `buttons` | Buttons 1–16 | Three buttons today; room to grow |

No left stick, triggers, or D-pad/hat in the report.

## Test app

A cross-platform MonoGame test harness lives in [`test-app/`](test-app/). It visualizes HID **Z** (spinner), **Rz** (paddle), and buttons for L3 Spinner / Paddle / Combo units, plus generic gamepads. See [`test-app/README.md`](test-app/README.md) for Linux dependencies (FreeType, BlueZ).

```bash
cd test-app
dotnet tool restore
dotnet run --project L3ControllerTest -f net9.0              # Linux
dotnet run --project L3ControllerTest -f net9.0-windows10.0.19041.0  # Windows
```

## Board definition credit

`boards/nicenano` variant derived from [bertrik/nicenano-example](https://github.com/bertrik/nicenano-example) (MIT).
