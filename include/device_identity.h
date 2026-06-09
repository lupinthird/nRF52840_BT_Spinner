#pragma once

// BLE GAP name, USB product string, and DIS serial per build environment.
// Edit DEVICE_SERIAL_STR for each variant when assigning unit / batch IDs.

#define MANUFACTURER_STR "L3 Labs"

#if DEVICE_VARIANT_ID == 0
#define DEVICE_DISPLAY_NAME "L3 Spinner Red"
#define USB_PRODUCT_NAME "L3 Spinner Red"
#define DEVICE_SERIAL_STR "L3-SPR-R-001"
#elif DEVICE_VARIANT_ID == 1
#define DEVICE_DISPLAY_NAME "L3 Spinner Blue"
#define USB_PRODUCT_NAME "L3 Spinner Blue"
#define DEVICE_SERIAL_STR "L3-SPR-B-001"
#elif DEVICE_VARIANT_ID == 2
#define DEVICE_DISPLAY_NAME "L3 Spinner Green"
#define USB_PRODUCT_NAME "L3 Spinner Green"
#define DEVICE_SERIAL_STR "L3-SPR-G-001"
#elif DEVICE_VARIANT_ID == 3
#define DEVICE_DISPLAY_NAME "L3 Spinner Yellow"
#define USB_PRODUCT_NAME "L3 Spinner Yellow"
#define DEVICE_SERIAL_STR "L3-SPR-Y-001"
#elif DEVICE_VARIANT_ID == 4
#define DEVICE_DISPLAY_NAME "L3 Paddle Red"
#define USB_PRODUCT_NAME "L3 Paddle Red"
#define DEVICE_SERIAL_STR "L3-PAD-R-001"
#elif DEVICE_VARIANT_ID == 5
#define DEVICE_DISPLAY_NAME "L3 Paddle Blue"
#define USB_PRODUCT_NAME "L3 Paddle Blue"
#define DEVICE_SERIAL_STR "L3-PAD-B-001"
#elif DEVICE_VARIANT_ID == 6
#define DEVICE_DISPLAY_NAME "L3 Paddle Green"
#define USB_PRODUCT_NAME "L3 Paddle Green"
#define DEVICE_SERIAL_STR "L3-PAD-G-001"
#elif DEVICE_VARIANT_ID == 7
#define DEVICE_DISPLAY_NAME "L3 Paddle Yellow"
#define USB_PRODUCT_NAME "L3 Paddle Yellow"
#define DEVICE_SERIAL_STR "L3-PAD-Y-001"
#elif DEVICE_VARIANT_ID == 8
#define DEVICE_DISPLAY_NAME "L3 Combo Red"
#define USB_PRODUCT_NAME "L3 Combo Red"
#define DEVICE_SERIAL_STR "L3-CMB-R-001"
#elif DEVICE_VARIANT_ID == 9
#define DEVICE_DISPLAY_NAME "L3 Combo Blue"
#define USB_PRODUCT_NAME "L3 Combo Blue"
#define DEVICE_SERIAL_STR "L3-CMB-B-001"
#elif DEVICE_VARIANT_ID == 10
#define DEVICE_DISPLAY_NAME "L3 Combo Green"
#define USB_PRODUCT_NAME "L3 Combo Green"
#define DEVICE_SERIAL_STR "L3-CMB-G-001"
#elif DEVICE_VARIANT_ID == 11
#define DEVICE_DISPLAY_NAME "L3 Combo Yellow"
#define USB_PRODUCT_NAME "L3 Combo Yellow"
#define DEVICE_SERIAL_STR "L3-CMB-Y-001"
#else
#define DEVICE_DISPLAY_NAME "L3 Controller"
#define USB_PRODUCT_NAME "L3 Controller"
#define DEVICE_SERIAL_STR "L3-UNK-000"
#endif
