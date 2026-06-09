#include "pot.h"

#include "config.h"

#if DEVICE_HAS_POT

#include "hardware/pins.h"

#include <Arduino.h>
#include <math.h>

void pot_init(void) {
  pinMode(PIN_VCC_EN, OUTPUT);
  digitalWrite(PIN_VCC_EN, HIGH);
}

int16_t pot_read_raw(void) {
  uint32_t sum = 0;
#if POT_AVG_SAMPLES > 1
  for (uint8_t i = 0; i < POT_AVG_SAMPLES; i++) {
    sum += analogRead(PIN_POT_Y_AXIS);
  }
  return static_cast<int16_t>(sum / POT_AVG_SAMPLES);
#else
  return analogRead(PIN_POT_Y_AXIS);
#endif
}

int8_t pot_read_axis(void) {
  const int16_t raw = pot_read_raw();

  const int16_t span_i = POT_ADC_MAX - POT_ADC_MIN;
  if (span_i <= 0) {
    return 0;
  }

  // Hard snap near calibrated limits — raw ADC dithers at mechanical stops.
  if (raw <= static_cast<int16_t>(POT_ADC_MIN + POT_END_SNAP_ADC)) {
#if POT_INVERT_Y_AXIS
    return HID_AXIS_MAX;
#else
    return HID_AXIS_MIN;
#endif
  }
  if (raw >= static_cast<int16_t>(POT_ADC_MAX - POT_END_SNAP_ADC)) {
#if POT_INVERT_Y_AXIS
    return HID_AXIS_MIN;
#else
    return HID_AXIS_MAX;
#endif
  }

  const float span = static_cast<float>(span_i);
  float norm = (raw - POT_ADC_MIN) / span;
  if (norm < 0.0f) {
    norm = 0.0f;
  } else if (norm > 1.0f) {
    norm = 1.0f;
  }

  int32_t axis = static_cast<int32_t>(lroundf(norm * 254.0f - 127.0f));
#if POT_INVERT_Y_AXIS
  axis = -axis;
#endif

  if (axis > HID_AXIS_MAX) {
    return HID_AXIS_MAX;
  }
  if (axis < HID_AXIS_MIN) {
    return HID_AXIS_MIN;
  }
  return static_cast<int8_t>(axis);
}

#if POT_DEBUG
void pot_debug_tick(int8_t axis) {
  static int16_t sweep_min = 4095;
  static int16_t sweep_max = 0;
  static uint32_t last_print_ms;

  const int16_t raw = pot_read_raw();
  if (raw < sweep_min) {
    sweep_min = raw;
  }
  if (raw > sweep_max) {
    sweep_max = raw;
  }

  const uint32_t now = millis();
  if ((now - last_print_ms) < 200) {
    return;
  }
  last_print_ms = now;

  Serial.print(F("pot raw="));
  Serial.print(raw);
  Serial.print(F(" sweep_min="));
  Serial.print(sweep_min);
  Serial.print(F(" sweep_max="));
  Serial.print(sweep_max);
  Serial.print(F(" axis="));
  Serial.println(axis);
}
#endif

#endif  // DEVICE_HAS_POT
