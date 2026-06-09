#pragma once

#include <stdint.h>

void ble_gamepad_init(void);
void ble_gamepad_update(uint32_t now_ms);
void ble_gamepad_send(int8_t axis_z, int8_t axis_rz, uint32_t button_mask);
bool ble_gamepad_is_connected(void);
