#pragma once

#include <stdint.h>

#include "class/hid/hid.h"

#ifndef HID_GAMEPAD_BUTTON_COUNT
#define HID_GAMEPAD_BUTTON_COUNT 16
#endif

enum { HID_GAMEPAD_REPORT_ID = 1 };

// Standard 6-axis gamepad report so Windows exposes one SDL device with axes
// and buttons together. Spinner maps to Z; paddle maps to Rz; combo uses both.
typedef struct TU_ATTR_PACKED {
  int8_t x;
  int8_t y;
  int8_t z;
  int8_t rx;
  int8_t ry;
  int8_t rz;
  uint16_t buttons;
} l3_gamepad_report_t;

static const uint8_t hid_gamepad_report_descriptor[] = {
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),
    HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(HID_GAMEPAD_REPORT_ID)
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),
    HID_USAGE(HID_USAGE_DESKTOP_X),
    HID_USAGE(HID_USAGE_DESKTOP_Y),
    HID_USAGE(HID_USAGE_DESKTOP_Z),
    HID_USAGE(HID_USAGE_DESKTOP_RX),
    HID_USAGE(HID_USAGE_DESKTOP_RY),
    HID_USAGE(HID_USAGE_DESKTOP_RZ),
    HID_LOGICAL_MIN(0x81),
    HID_LOGICAL_MAX(0x7f),
    HID_REPORT_COUNT(6),
    HID_REPORT_SIZE(8),
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
    HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),
    HID_USAGE_MIN(1),
    HID_USAGE_MAX(HID_GAMEPAD_BUTTON_COUNT),
    HID_LOGICAL_MIN(0),
    HID_LOGICAL_MAX(1),
    HID_REPORT_COUNT(HID_GAMEPAD_BUTTON_COUNT),
    HID_REPORT_SIZE(1),
    HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
    HID_COLLECTION_END};

static const uint16_t hid_gamepad_report_size = sizeof(l3_gamepad_report_t);
