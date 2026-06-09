#include "encoder_qdec.h"

#include "config.h"
#include "hardware/pins.h"

#include <Arduino.h>

#include <nrf.h>
#include <nrf_gpio.h>

#ifndef ENCODER_QDEC_SAMPLEPER
#define ENCODER_QDEC_SAMPLEPER QDEC_SAMPLEPER_SAMPLEPER_128us
#endif

static int32_t abs_counts;
static int32_t last_report_counts;

static uint32_t encoder_nrf_pin_a(void) {
  return g_ADigitalPinMap[PIN_ENCODER_A];
}

static uint32_t encoder_nrf_pin_b(void) {
  return g_ADigitalPinMap[PIN_ENCODER_B];
}

static void qdec_stop(void) {
  if (NRF_QDEC->ENABLE) {
    NRF_QDEC->TASKS_STOP = 1;
    while (NRF_QDEC->EVENTS_STOPPED == 0) {
    }
    NRF_QDEC->EVENTS_STOPPED = 0;
    NRF_QDEC->ENABLE = 0;
  }
}

static void encoder_gpio_configure(void) {
#if ENCODER_INPUT_PULLUP
  const nrf_gpio_pin_pull_t pull = NRF_GPIO_PIN_PULLUP;
#else
  const nrf_gpio_pin_pull_t pull = NRF_GPIO_PIN_NOPULL;
#endif
  nrf_gpio_cfg_input(encoder_nrf_pin_a(), pull);
  nrf_gpio_cfg_input(encoder_nrf_pin_b(), pull);
}

static int8_t read_ab_state(void) {
  const uint8_t a = nrf_gpio_pin_read(encoder_nrf_pin_a()) ? 1 : 0;
  const uint8_t b = nrf_gpio_pin_read(encoder_nrf_pin_b()) ? 1 : 0;
  return static_cast<int8_t>((a << 1) | b);
}

static int8_t quadrature_delta(int8_t prev, int8_t curr) {
  static const int8_t table[16] = {
      0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0,
  };
  const uint8_t index =
      static_cast<uint8_t>(((prev & 0x03) << 2) | (curr & 0x03));
  return table[index];
}

#if ENCODER_BACKEND_SOFTWARE
static int32_t sw_position;
static int8_t sw_last_ab;

static void software_poll(void) {
  const int8_t ab = read_ab_state();
  const int8_t step = quadrature_delta(sw_last_ab, ab);
  sw_last_ab = ab;
  if (step != 0) {
    sw_position += step;
  }
}

static void software_init(void) {
  qdec_stop();
  encoder_gpio_configure();
  sw_position = 0;
  sw_last_ab = read_ab_state();
  last_report_counts = 0;
}

static int32_t software_consume_delta(void) {
  software_poll();
  const int32_t position = sw_position;
  const int32_t delta = position - last_report_counts;
  last_report_counts = position;
  return delta;
}

static int32_t software_get_position(void) {
  return sw_position;
}

#else  // hardware QDEC

static void qdec_hw_init(void) {
  qdec_stop();
  encoder_gpio_configure();

  NRF_QDEC->INTENCLR = 0xFFFFFFFF;
  NRF_QDEC->SAMPLEPER = ENCODER_QDEC_SAMPLEPER;
  NRF_QDEC->PSEL.A = encoder_nrf_pin_a();
  NRF_QDEC->PSEL.B = encoder_nrf_pin_b();
  NRF_QDEC->PSEL.LED =
      (QDEC_PSEL_LED_CONNECT_Disconnected << QDEC_PSEL_LED_CONNECT_Pos);
  NRF_QDEC->DBFEN = 0;
  NRF_QDEC->SHORTS = 0;

  abs_counts = 0;
  last_report_counts = 0;
  NRF_QDEC->TASKS_RDCLRACC = 1;

  NRF_QDEC->ENABLE = 1;
  NRF_QDEC->TASKS_START = 1;
}

static int32_t qdec_consume_delta(void) {
  if (!NRF_QDEC->ENABLE) {
    return 0;
  }

  NRF_QDEC->TASKS_RDCLRACC = 1;
  const int32_t acc = -(int32_t)NRF_QDEC->ACCREAD;
  abs_counts += acc;

  const int32_t position = abs_counts / 2;
  const int32_t delta = position - last_report_counts;
  last_report_counts = position;
  return delta;
}

static int32_t qdec_get_position(void) {
  return abs_counts / 2;
}

#endif  // ENCODER_BACKEND_SOFTWARE

static int32_t apply_direction(int32_t value) {
#if ENCODER_INVERT_DIRECTION
  return -value;
#else
  return value;
#endif
}

#if ENCODER_PIN_SCAN
// Candidate P0 pins on nice!nano (skip USB/battery if possible).
static const uint8_t kScanPins[] = {
    2,  3,  4,  5,  6,  8,  9,  10, 11, 15,
    17, 20, 22, 24, 29, 31,
};

static uint16_t scan_hi_mask;
static uint16_t scan_lo_mask;
static uint16_t scan_toggled_mask;
static uint8_t scan_last_state[16];
static bool scan_initialized;

void encoder_pin_scan_tick(void) {
  for (uint8_t i = 0; i < sizeof(kScanPins); i++) {
    const uint8_t p = kScanPins[i];
    nrf_gpio_cfg_input(p, NRF_GPIO_PIN_NOPULL);
    const uint8_t level = nrf_gpio_pin_read(p) ? 1 : 0;
    if (scan_initialized && level != scan_last_state[i]) {
      scan_toggled_mask |= (1U << i);
    }
    scan_last_state[i] = level;
    if (level) {
      scan_hi_mask |= (1U << i);
    } else {
      scan_lo_mask |= (1U << i);
    }
  }
  scan_initialized = true;
}

static void encoder_pin_scan_report(void) {
  Serial.print(F("pinscan toggled["));
  for (uint8_t i = 0; i < sizeof(kScanPins); i++) {
    if (scan_toggled_mask & (1U << i)) {
      Serial.print(F("P0."));
      Serial.print(kScanPins[i]);
      Serial.print(F(" "));
    }
  }
  Serial.print(F("] hi["));
  for (uint8_t i = 0; i < sizeof(kScanPins); i++) {
    if (scan_hi_mask & (1U << i)) {
      Serial.print(F("P0."));
      Serial.print(kScanPins[i]);
      Serial.print(F(" "));
    }
  }
  Serial.print(F("] lo["));
  for (uint8_t i = 0; i < sizeof(kScanPins); i++) {
    if (scan_lo_mask & (1U << i)) {
      Serial.print(F("P0."));
      Serial.print(kScanPins[i]);
      Serial.print(F(" "));
    }
  }
  Serial.println(F("]  (toggled[] persists until reset — set pins.h to that pair)"));
  scan_hi_mask = 0;
  scan_lo_mask = 0;
}
#endif

#if ENCODER_DEBUG
static void encoder_probe_pins(void) {
  const uint32_t nrf_a = encoder_nrf_pin_a();
  const uint32_t nrf_b = encoder_nrf_pin_b();

  Serial.println(F("--- Encoder GPIO probe ---"));
  Serial.print(F("Configured: Arduino A="));
  Serial.print(PIN_ENCODER_A);
  Serial.print(F(" B="));
  Serial.print(PIN_ENCODER_B);
  Serial.print(F(" -> nRF P0."));
  Serial.print(nrf_a);
  Serial.print(F(" / P0."));
  Serial.println(nrf_b);
  Serial.println(F("Confirmed wiring: P0.02 (A) + P0.29 (B) — see hardware/pins.h"));

  nrf_gpio_cfg_input(nrf_a, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(nrf_b, NRF_GPIO_PIN_PULLUP);
  delay(5);
  Serial.print(F("Pull-up test: A="));
  Serial.print(nrf_gpio_pin_read(nrf_a));
  Serial.print(F(" B="));
  Serial.println(nrf_gpio_pin_read(nrf_b));

  encoder_gpio_configure();
  delay(5);
  Serial.print(F("No pull-up:   A="));
  Serial.print(nrf_gpio_pin_read(nrf_a));
  Serial.print(F(" B="));
  Serial.println(nrf_gpio_pin_read(nrf_b));
  Serial.println(F("--- end probe ---"));
}
#endif

void encoder_init(void) {
#if ENCODER_DEBUG
  encoder_probe_pins();
#endif
#if ENCODER_BACKEND_SOFTWARE
  software_init();
#else
  qdec_hw_init();
#endif
}

void encoder_reset_position(void) {
#if ENCODER_BACKEND_SOFTWARE
  sw_position = 0;
  sw_last_ab = read_ab_state();
#else
  abs_counts = 0;
  if (NRF_QDEC->ENABLE) {
    NRF_QDEC->TASKS_RDCLRACC = 1;
  }
#endif
  last_report_counts = 0;
}

void encoder_update(int32_t* delta, int32_t* position) {
  int32_t d;
  int32_t p;

#if ENCODER_BACKEND_SOFTWARE
  d = software_consume_delta();
  p = software_get_position();
#else
  d = qdec_consume_delta();
  p = qdec_get_position();
#endif

  d = apply_direction(d);
  p = apply_direction(p);

  if (delta) {
    *delta = d;
  }
  if (position) {
    *position = p;
  }
}

int32_t encoder_get_position(void) {
  int32_t p = 0;
  encoder_update(nullptr, &p);
  return p;
}

int32_t encoder_poll_delta(void) {
  int32_t d = 0;
  encoder_update(&d, nullptr);
  return d;
}

#if ENCODER_DEBUG
void encoder_debug_print(int32_t last_delta, int8_t stick_x) {
  const uint8_t a_raw = nrf_gpio_pin_read(encoder_nrf_pin_a());
  const uint8_t b_raw = nrf_gpio_pin_read(encoder_nrf_pin_b());

  Serial.print(F("enc P0."));
  Serial.print(encoder_nrf_pin_a());
  Serial.print(F("="));
  Serial.print(a_raw);
  Serial.print(F(" P0."));
  Serial.print(encoder_nrf_pin_b());
  Serial.print(F("="));
  Serial.print(b_raw);
  Serial.print(F(" delta="));
  Serial.print(last_delta);
  Serial.print(F(" pos="));
#if ENCODER_BACKEND_SOFTWARE
  Serial.print(sw_position);
#else
  Serial.print(abs_counts / 2);
  Serial.print(F(" qdec="));
  Serial.print(NRF_QDEC->ENABLE);
#endif
  Serial.print(F(" stick_x="));
  Serial.println(stick_x);

#if ENCODER_PIN_SCAN
  encoder_pin_scan_report();
#endif
}
#endif
