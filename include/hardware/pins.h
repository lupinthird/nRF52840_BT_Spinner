#pragma once

#include <stdint.h>
#include <variant.h>

#include "config.h"

// ---------------------------------------------------------------------------
// Encoder A/B -> nRF52840 after 5 V -> 3.3 V divider:
//   Encoder signal --[ R_top ]-- MCU pin --[ R_bottom ]-- GND
//   10k / 22k -> ~69% of encoder high (~2.75 V at 4 V); Bourns is push-pull (no pull-up).
//
// Bourns ENA1J axial pins: GND | N/C | A | + (Vcc) | B
//
// Confirmed wiring (pin scan + QDEC working):
//   Channel A -> P0.02 (PIN_002)
//   Channel B -> P0.29 (PIN_029)
// ---------------------------------------------------------------------------

#define PIN_ENCODER_A PIN_002  // nRF P0.02
#define PIN_ENCODER_B PIN_029  // nRF P0.29

// Buttons: active low with internal pull-up (switch to GND).
#define PIN_BUTTON_0 PIN_100  // P1.00
#define PIN_BUTTON_1 PIN_024  // P0.24
#define PIN_BUTTON_2 PIN_022  // P0.22

#define PIN_POT_Y_AXIS PIN_031  // P0.31 / AIN7

#if DEVICE_HAS_POT
// Enables the nice!nano 3.3 V VCC pin for pot top rail (HIGH on v2).
#define PIN_VCC_EN PIN_013  // P0.13
#endif

// Reserved on nice!nano v1 (onboard divider from B+). v2 uses VDDH SAADC instead — do not wire here.
#define PIN_BATTERY PIN_004  // P0.04 / AIN2

static const uint8_t BUTTON_PINS[] = {
    PIN_BUTTON_0,
    PIN_BUTTON_1,
    PIN_BUTTON_2,
};

static const uint8_t BUTTON_COUNT =
    sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);
