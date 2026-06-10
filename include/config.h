#pragma once

// Variant ID is set per PlatformIO environment (-DDEVICE_VARIANT_ID=N).
//   0–3: Spinner (Red, Blue, Green, Yellow)
//   4–7: Paddle  (Red, Blue, Green, Yellow)
//   8–11: Combo   (Red, Blue, Green, Yellow) — spinner + pot
#ifndef DEVICE_VARIANT_ID
#define DEVICE_VARIANT_ID 0
#endif

#if DEVICE_VARIANT_ID >= 8
#define DEVICE_HAS_ENCODER 1
#define DEVICE_HAS_POT 1
#elif DEVICE_VARIANT_ID >= 4
#define DEVICE_HAS_ENCODER 0
#define DEVICE_HAS_POT 1
#else
#define DEVICE_HAS_ENCODER 1
#define DEVICE_HAS_POT 0
#endif

#include "device_identity.h"

// Bourns ENA1J-B28-L00064L: 64 quadrature cycles per revolution.
#define ENCODER_QUADRATURE_CYCLES_PER_REV 64
#define ENCODER_COUNTS_PER_REV (ENCODER_QUADRATURE_CYCLES_PER_REV * 4)

#define MAIN_LOOP_INTERVAL_MS 8

#define HID_AXIS_MIN (-127)
#define HID_AXIS_MAX 127

#define ENCODER_COUNTS_FOR_FULL_DEFLECTION 256

// HID axis wraps every this many encoder counts; internal position keeps accumulating.

// 0 = position (arcade spinner — axis holds shaft angle; pairs with VectorBreakout)
// 1 = velocity + spring decay (thumbstick-like; legacy bring-up mode)
#ifndef ENCODER_MAP_VELOCITY
#define ENCODER_MAP_VELOCITY 0
#endif

// Set to 1 if spinner rotation sense is opposite in Windows (QDEC vs Bourns CW).
#ifndef ENCODER_INVERT_DIRECTION
#define ENCODER_INVERT_DIRECTION 0
#endif

// QDEC sample period: set ENCODER_QDEC_SAMPLEPER in encoder_qdec.cpp (nrf52 enum).

#ifndef ENCODER_BACKEND_SOFTWARE
#define ENCODER_BACKEND_SOFTWARE 0
#endif

// Set to 1 while bring-up; turn off once encoder and joy.cpl look good.
#ifndef ENCODER_DEBUG
#define ENCODER_DEBUG 0
#endif

// Spinner feel: only used when ENCODER_MAP_VELOCITY is 1.
#define ENCODER_VELOCITY_GAIN 6
#define ENCODER_VELOCITY_DECAY 8

// Bourns ENA1J is push-pull — normally use NOPULL (0) with the 1k/2k divider.
// Pull-ups are for open-collector encoders; try 1 only as a bring-up experiment.
#ifndef ENCODER_INPUT_PULLUP
#define ENCODER_INPUT_PULLUP 0
#endif

// Set to 1 only during wiring bring-up (reconfigures GPIO every loop — disable when done).
#ifndef ENCODER_PIN_SCAN
#define ENCODER_PIN_SCAN 0
#endif

// Battery: nice!nano V2 uses nRF52840 VDDH/5 SAADC; Feather uses A7 divider (see ble_gamepad.cpp).
#define BATT_V_MIN 3.0f
#define BATT_V_MAX 4.2f
#define BATT_UPDATE_MS 15000

#ifndef BATT_DEBUG
#define BATT_DEBUG 0
#endif

#if DEVICE_HAS_POT
#include "paddle_calibration.h"
#ifndef POT_ADC_MIN
#define POT_ADC_MIN 0
#endif
#ifndef POT_ADC_MAX
#define POT_ADC_MAX 4095
#endif
#define POT_AVG_SAMPLES 8
#ifndef POT_END_SNAP_ADC
// Raw counts within this margin of calibrated end stops snap to full deflection.
#define POT_END_SNAP_ADC 16
#endif
#ifndef POT_INVERT_Y_AXIS
#define POT_INVERT_Y_AXIS 0
#endif
#ifndef POT_DEBUG
#define POT_DEBUG 0
#endif
#endif
