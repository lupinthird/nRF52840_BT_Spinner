# L3 Controller Test App

Windowed MonoGame test application for verifying BLE HID controllers built from the **nRF52840_BT_Spinner** firmware in this repo.

## Requirements

- [.NET 9 SDK](https://dotnet.microsoft.com/download/dotnet/9.0)
- **Windows 10+** or **Linux** (DesktopGL / SDL2)
- [`L3ControllerInput`](../../L3ControllerInput) as a sibling directory of this repo (see project reference path in `L3ControllerTest.csproj`)

### Linux-only build dependency

MGCB needs FreeType to compile the bundled spritefont:

```bash
sudo apt install libfreetype6 libfreetype6-dev   # Debian / Ubuntu
# or: sudo dnf install freetype-devel            # Fedora
# or: sudo pacman -S freetype2                   # Arch
```

On Linux, BLE serial lookup requires **BlueZ** (`bluez` package) and a connected L3 controller.

## Build and run

From this directory:

```bash
cd test-app
dotnet tool restore
```

**Linux:**

```bash
dotnet run --project L3ControllerTest -f net9.0
```

**Windows:**

```powershell
dotnet run --project L3ControllerTest -f net9.0-windows10.0.19041.0
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

Input is handled by [`L3ControllerInput`](../../L3ControllerInput). Platform behavior:

| Platform | Primary path | Fallback |
|---|---|---|
| **Windows** | `RawGameController` (HID Z/Rz) | MonoGame `Joystick`, then `Gamepad` |
| **Linux** | MonoGame `Joystick` (SDL2) | BLE GATT serial via BlueZ |

L3 devices do **not** use left-stick X/Y mapping. Generic gamepads on Windows still fall back to stick axes when Z/Rz are unavailable.

## Project layout

```
test-app/
├── L3ControllerTest.sln
├── .config/dotnet-tools.json
└── L3ControllerTest/          # Visual test harness (UI only)
    ├── Game1.cs
    ├── Content/
    │   └── Fonts/
    │       ├── Default.spritefont
    │       └── VictorMono-Regular.ttf
    └── UI/
        ├── SlotPanel.cs
        ├── SpinnerGauge.cs
        ├── PaddleGauge.cs
        ├── StickIndicator.cs
        └── ButtonGrid.cs

../../L3ControllerInput/         # Reusable claim + mapping library (sibling repo)
    ├── L3DeviceInfo.cs
    ├── L3ControllerIdentity.cs
    ├── InputMapping.cs
    ├── ControllerManager.cs
    ├── ControllerSnapshot.cs
    └── TrackedController.cs
```

Game projects reference `../../../L3ControllerInput/L3ControllerInput.csproj` — see that folder's README.

## Notes

- Slot assignments are not persisted across launches.
- Spinner uses wrap-aware delta integration on Z axis changes (matches encoder position reporting in firmware).
- The UI font (`VictorMono-Regular.ttf`) is bundled in `Content/Fonts/` so MGCB builds identically on Windows and Linux.
