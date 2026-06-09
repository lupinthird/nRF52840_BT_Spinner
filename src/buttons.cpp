#include "buttons.h"

#include "hardware/pins.h"

#include <Arduino.h>

static const uint32_t DEBOUNCE_MS = 5;

static uint32_t last_stable_mask;
static uint32_t last_change_ms[BUTTON_COUNT];

static uint32_t read_raw_mask(void) {
  uint32_t mask = 0;
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {
      mask |= (1UL << i);
    }
  }
  return mask;
}

void buttons_init(void) {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    last_change_ms[i] = 0;
  }
  last_stable_mask = read_raw_mask();
}

uint32_t buttons_read_mask(void) {
  const uint32_t raw = read_raw_mask();
  const uint32_t now = millis();
  uint32_t stable = last_stable_mask;

  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    const uint32_t bit = (1UL << i);
    const bool raw_pressed = (raw & bit) != 0;
    const bool stable_pressed = (stable & bit) != 0;

    if (raw_pressed != stable_pressed) {
      if (last_change_ms[i] == 0) {
        last_change_ms[i] = now;
      } else if ((now - last_change_ms[i]) >= DEBOUNCE_MS) {
        if (raw_pressed) {
          stable |= bit;
        } else {
          stable &= ~bit;
        }
        last_change_ms[i] = 0;
      }
    } else {
      last_change_ms[i] = 0;
    }
  }

  last_stable_mask = stable;
  return stable;
}
