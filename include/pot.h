#pragma once

#include <stdint.h>

#include "config.h"

#if DEVICE_HAS_POT
void pot_init(void);
int16_t pot_read_raw(void);
int8_t pot_read_axis(void);
#if POT_DEBUG
void pot_debug_tick(int8_t axis);
#endif
#endif
