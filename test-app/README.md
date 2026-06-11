# L3 Controller Test App

Windowed MonoGame test application for verifying BLE HID controllers built from the **nRF52840_BT_Spinner** firmware in this repo.

## Requirements

- Windows 10 or later
- [.NET 9 SDK](https://dotnet.microsoft.com/download/dotnet/9.0)

## Build and run

From this directory:

```powershell
cd test-app
dotnet tool restore
dotnet run --project L3ControllerTest
```

Press **Escape** to exit.

## Firmware HID layout

The firmware in [`include/hid_gamepad_descriptor.h`](../include/hid_gamepad_descriptor.h) exposes a **6-axis desktop gamepad** report:

| HID field | Firmware source | Test app widget |
|---|---|---|
| **Z** | Rotary encoder (spinner variants) | Spinner gauge (wrap-aware integrated angle) |
| **Rz** | Potentiometer (paddle/combo variants) | Paddle gauge (bounded L/R stops) |
| **Buttons 1–3** | Three front-panel buttons | Button grid (Btn 1–3) |

Variant BLE names (from [`include/device_identity.h`](../include/device_identity.h)):

| Profile | Example name | Axes active |
|---|---|---|
| Spinner | `L3 Spinner Red` | Z |
| Paddle | `L3 Paddle Blue` | Rz |
| Combo | `L3 Combo Green` | Z + Rz |

Build the matching PlatformIO environment (`spinner_red`, `paddle_blue`, `combo_yellow`, etc.) before testing that unit.

## What the app shows

1280×720 window, 2×2 grid, up to **four controllers**. Slots stay hidden until first input activity claims the next free slot (buttons, Z axis, or Rz axis).

Each claimed slot shows:

- Firmware **serial** (line 1) and parsed **display name** (line 2, color accent)
- **Type** and **unit** from `L3DeviceInfo` (line 3)
- Button states (3 buttons for L3 devices; full gamepad layout for Xbox-style pads)
- **Spinner gauge** — circular dial with rotating nib (from HID **Z**)
- **Paddle gauge** — dial with L/R stop ticks (from HID **Rz**)
- Stick crosshair and triggers — generic gamepads only

## Input sources

The app polls three Windows input APIs:

1. **`RawGameController`** — primary path for L3 HID Z/Rz axes (indices 2 and 5)
2. **MonoGame `Joystick`** — fallback when SDL exposes the same desktop axis layout
3. **`Gamepad`** — Xbox-style pads (stick X/Y fallback for spinner/paddle widgets)

L3 devices do **not** use left-stick X/Y mapping. Generic gamepads still fall back to stick axes when Z/Rz are unavailable.

## Project layout

```
test-app/
├── L3ControllerTest.sln
├── .config/dotnet-tools.json
└── L3ControllerTest/          # Visual test harness (UI only)
    ├── Game1.cs
    └── UI/
        ├── SlotPanel.cs
        ├── SpinnerGauge.cs
        ├── PaddleGauge.cs
        ├── StickIndicator.cs
        └── ButtonGrid.cs

../L3ControllerInput/         # Reusable claim + mapping library (Cursor Projects root)
    ├── L3DeviceInfo.cs
    ├── L3ControllerIdentity.cs
    ├── InputMapping.cs
    ├── ControllerManager.cs
    ├── ControllerSnapshot.cs
    └── TrackedController.cs
```

Game projects reference `../L3ControllerInput/L3ControllerInput.csproj` — see that folder's README.

## Notes

- Windows only (`Windows.Gaming.Input` WinRT APIs).
- Slot assignments are not persisted across launches.
- Spinner uses wrap-aware delta integration on Z axis changes (matches encoder position reporting in firmware).
