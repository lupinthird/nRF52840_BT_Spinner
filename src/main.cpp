#include <Arduino.h>

#include "ble_gamepad.h"
#include "buttons.h"
#include "config.h"

#if DEVICE_HAS_ENCODER
#include "encoder_qdec.h"
#endif
#if DEVICE_HAS_POT
#include "pot.h"
#endif

#if DEVICE_HAS_ENCODER

// Map encoder motion to a stick axis using velocity (thumbstick-like spring return).
// Used only when ENCODER_MAP_VELOCITY is 1.
static int8_t encoder_velocity_to_axis(int32_t delta, int8_t axis) {
  if (delta != 0) {
    int32_t value = axis + (delta * ENCODER_VELOCITY_GAIN);
    if (value > HID_AXIS_MAX) {
      value = HID_AXIS_MAX;
    } else if (value < HID_AXIS_MIN) {
      value = HID_AXIS_MIN;
    }
    return static_cast<int8_t>(value);
  }

  if (axis > 0) {
    axis = static_cast<int8_t>(axis - ENCODER_VELOCITY_DECAY);
    if (axis < 0) {
      axis = 0;
    }
  } else if (axis < 0) {
    axis = static_cast<int8_t>(axis + ENCODER_VELOCITY_DECAY);
    if (axis > 0) {
      axis = 0;
    }
  }
  return axis;
}

// Map accumulated encoder counts to stick deflection with wrap (no hard stop).
static int32_t wrap_spinner_position(int32_t position) {
  const int32_t span = ENCODER_COUNTS_FOR_FULL_DEFLECTION;
  if (span <= 0) {
    return 0;
  }

  int32_t wrapped = position % span;
  if (wrapped < 0) {
    wrapped += span;
  }
  if (wrapped > span / 2) {
    wrapped -= span;
  }
  return wrapped;
}

static int8_t encoder_position_to_axis(int32_t position) {
  const int32_t half_range = ENCODER_COUNTS_FOR_FULL_DEFLECTION / 2;
  if (half_range <= 0) {
    return 0;
  }

  const int32_t wrapped = wrap_spinner_position(position);
  const int32_t value = (wrapped * HID_AXIS_MAX) / half_range;
  return static_cast<int8_t>(value);
}

#endif  // DEVICE_HAS_ENCODER

void setup(void) {
  Serial.begin(115200);
  delay(100);

  Serial.println(DEVICE_DISPLAY_NAME);
  Serial.print(F("Variant ID: "));
  Serial.println(DEVICE_VARIANT_ID);
  Serial.println(F("Pair via Bluetooth, then use joy.cpl or a gamepad tester."));
#if DEVICE_HAS_ENCODER && DEVICE_HAS_POT
  Serial.println(F("Profile: combo (spinner X + pot Y + buttons)"));
#elif DEVICE_HAS_POT
  Serial.println(F("Profile: paddle (pot Y + buttons)"));
#else
#if ENCODER_DEBUG
  Serial.print(F("Input mode: "));
  Serial.println(ENCODER_INPUT_PULLUP ? F("PULLUP") : F("NOPULL"));
#endif
#if ENCODER_BACKEND_SOFTWARE
  Serial.println(F("Encoder backend: software GPIO"));
#else
  Serial.println(F("Encoder backend: hardware QDEC"));
#endif
#if ENCODER_DEBUG
  Serial.println(F("Encoder debug enabled (115200 serial)."));
#endif
#if ENCODER_PIN_SCAN
  Serial.println(F("Pin scan enabled — spin encoder; watch pinscan lines."));
#endif
#endif

  buttons_init();
  analogReadResolution(12);
#if DEVICE_HAS_ENCODER
  encoder_init();
#endif
#if DEVICE_HAS_POT
  pot_init();
  Serial.print(F("Pot ADC range: "));
  Serial.print(POT_ADC_MIN);
  Serial.print(F(" .. "));
  Serial.println(POT_ADC_MAX);
#if POT_DEBUG
  Serial.println(F("Pot debug: sweep to both stops; use sweep_min/max as POT_ADC_MIN/MAX."));
#endif
#endif
  ble_gamepad_init();
}

void loop(void) {
  static uint32_t last_ms;
#if ENCODER_DEBUG && DEVICE_HAS_ENCODER
  static uint32_t last_debug_ms;
#endif
  static int8_t stick_x;
  static int8_t stick_y;
#if DEVICE_HAS_ENCODER
  static int32_t spinner_position;
#endif

  const uint32_t now = millis();
  ble_gamepad_update(now);

  if ((now - last_ms) < MAIN_LOOP_INTERVAL_MS) {
    return;
  }
  last_ms = now;

#if DEVICE_HAS_ENCODER
  int32_t delta = 0;
  encoder_update(&delta, nullptr);
#if ENCODER_MAP_VELOCITY
  stick_x = encoder_velocity_to_axis(delta, stick_x);
#else
  spinner_position += delta;
  stick_x = encoder_position_to_axis(spinner_position);
#endif
#else
  stick_x = 0;
#endif

#if DEVICE_HAS_POT
  stick_y = pot_read_axis();
#else
  stick_y = 0;
#endif

#if ENCODER_PIN_SCAN && DEVICE_HAS_ENCODER
  encoder_pin_scan_tick();
#endif

  const uint32_t buttons = buttons_read_mask();
  ble_gamepad_send(stick_x, stick_y, buttons);

#if ENCODER_DEBUG && DEVICE_HAS_ENCODER
  if ((now - last_debug_ms) >= 500) {
    last_debug_ms = now;
    encoder_debug_print(delta, stick_x);
  }
#endif

#if POT_DEBUG && DEVICE_HAS_POT
  pot_debug_tick(stick_y);
#endif
}
