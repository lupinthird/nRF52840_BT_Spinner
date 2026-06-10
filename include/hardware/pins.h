#pragma once

#include <stdint.h>
#include <variant.h>

#include "config.h"

// ---------------------------------------------------------------------------
// Encoder A/B -> nRF52 after 5 V -> 3.3 V divider:
//   Encoder signal --[ R_top ]-- MCU pin --[ R_bottom ]-- GND
//   10k / 22k -> ~69% of encoder high (~2.75 V at 4 V); Bourns is push-pull (no pull-up).
//
// Bourns ENA1J axial pins: GND | N/C | A | + (Vcc) | B
//
// Confirmed wiring (pin scan + QDEC working on nice!nano):
//   Channel A -> P0.02
//   Channel B -> P0.29
//
// Feather nRF52832 proto layout (encoder A/B, three buttons, optional clear).
// ---------------------------------------------------------------------------

#if defined(BOARD_FEATHER_NRF52832)

#define PIN_ENCODER_A 2   // encoderPinA / A0 / P0.02
#define PIN_ENCODER_B 3   // encoderPinB / A1 / P0.03

// Buttons: active low with internal pull-up (switch to GND).
#define PIN_BUTTON_0 PIN_A5  // P0.29
#define PIN_BUTTON_1 PIN_A3  // P0.05
#define PIN_BUTTON_2 PIN_A4  // P0.28

#define PIN_CLEAR_BUTTON 8  // clearButton / RX / P0.08

// A7 (P0.31) is hard-wired to the Feather LiPo divider — use A6 for the pot.
#define PIN_POT_Y_AXIS PIN_A6  // P0.30

#else  // BOARD_NICENANO (default)

#define PIN_ENCODER_A PIN_002  // nRF P0.02
#define PIN_ENCODER_B PIN_029  // nRF P0.29

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

#endif  // BOARD_FEATHER_NRF52832

static const uint8_t BUTTON_PINS[] = {
    PIN_BUTTON_0,
    PIN_BUTTON_1,
    PIN_BUTTON_2,
};

static const uint8_t BUTTON_COUNT =
    sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);
