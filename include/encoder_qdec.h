#pragma once

#include <stdint.h>

#include "config.h"

void encoder_init(void);
void encoder_reset_position(void);

// Call once per main-loop tick. Reads hardware once and returns delta + position.
void encoder_update(int32_t* delta, int32_t* position);

// Legacy wrappers (each calls encoder_update internally — prefer encoder_update).
int32_t encoder_get_position(void);
int32_t encoder_poll_delta(void);

#if ENCODER_DEBUG
void encoder_debug_print(int32_t last_delta, int8_t stick_x);
#endif

#if ENCODER_PIN_SCAN
void encoder_pin_scan_tick(void);
#endif
