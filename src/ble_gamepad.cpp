#include "ble_gamepad.h"

#include "config.h"
#include "hid_gamepad_descriptor.h"

#include <Arduino.h>
#include <bluefruit.h>
#include <nrf.h>

static BLEDis bledis;
static BLEBas blebas;
static BLEHidGeneric blehid(1, 0, 0);

// nice!nano V2 routes LiPo to nRF52840 VDDH; SAADC channel VDDHDIV5 = VDDH / 5.
static float read_battery_volts(void) {
  volatile int16_t raw_value = 0;

  NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled;
  NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;
  NRF_SAADC->CH[0].CONFIG =
      (SAADC_CH_CONFIG_GAIN_Gain1_4 << SAADC_CH_CONFIG_GAIN_Pos) |
      (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |
      (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos);
  NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDDHDIV5;
  NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC;

  NRF_SAADC->RESULT.PTR = (uint32_t)&raw_value;
  NRF_SAADC->RESULT.MAXCNT = 1;

  NRF_SAADC->EVENTS_STARTED = 0;
  NRF_SAADC->TASKS_START = 1;
  while (NRF_SAADC->EVENTS_STARTED == 0) {
  }
  NRF_SAADC->EVENTS_STARTED = 0;

  NRF_SAADC->EVENTS_END = 0;
  NRF_SAADC->TASKS_SAMPLE = 1;
  while (NRF_SAADC->EVENTS_END == 0) {
  }
  NRF_SAADC->EVENTS_END = 0;

  NRF_SAADC->EVENTS_STOPPED = 0;
  NRF_SAADC->TASKS_STOP = 1;
  while (NRF_SAADC->EVENTS_STOPPED == 0) {
  }
  NRF_SAADC->EVENTS_STOPPED = 0;

  NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Disabled;

  const uint16_t raw = static_cast<uint16_t>(raw_value);
  return (static_cast<float>(raw) * 2.4f / 4095.0f * 5.0f);
}

static uint8_t battery_percent_from_volts(float volts) {
  const float pct =
      (volts - BATT_V_MIN) / (BATT_V_MAX - BATT_V_MIN) * 100.0f;
  if (pct < 0.0f) {
    return 0;
  }
  if (pct > 100.0f) {
    return 100;
  }
  return static_cast<uint8_t>(pct);
}

static uint8_t read_battery_percent(void) {
  return battery_percent_from_volts(read_battery_volts());
}

static void update_battery_service(uint32_t now_ms, uint32_t* last_update_ms) {
  if ((now_ms - *last_update_ms) < BATT_UPDATE_MS) {
    return;
  }
  *last_update_ms = now_ms;

  const float volts = read_battery_volts();
  const uint8_t level = battery_percent_from_volts(volts);
  blebas.write(level);
#if BATT_DEBUG
  Serial.print(F("batt "));
  Serial.print(volts, 2);
  Serial.print(F("V -> "));
  Serial.print(level);
  Serial.println(F("%"));
#endif
  if (ble_gamepad_is_connected()) {
    blebas.notify(level);
  }
}

static void start_advertising(void) {
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.ScanResponse.clearData();

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_GAMEPAD);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addService(blebas);

  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.start();
}

void ble_gamepad_init(void) {
  Bluefruit.begin(1, 0);
  Bluefruit.setTxPower(4);
  Bluefruit.setName(DEVICE_DISPLAY_NAME);

  bledis.setManufacturer(MANUFACTURER_STR);
  bledis.setModel(DEVICE_DISPLAY_NAME);
  bledis.setSerialNum(DEVICE_SERIAL_STR);
  bledis.begin();

  blebas.begin();
  blebas.write(read_battery_percent());

  uint16_t input_len[] = {hid_gamepad_report_size};

  blehid.setReportLen(input_len);
  blehid.enableKeyboard(false);
  blehid.enableMouse(false);
  blehid.setReportMap(hid_gamepad_report_descriptor,
                      sizeof(hid_gamepad_report_descriptor));
  blehid.begin();

  // HID gamepad-friendly connection interval (same as BLEHidGamepad).
  Bluefruit.Periph.setConnInterval(9, 12);

  start_advertising();
}

void ble_gamepad_update(uint32_t now_ms) {
  static uint32_t last_battery_ms;

  update_battery_service(now_ms, &last_battery_ms);
}

void ble_gamepad_send(int8_t axis_z, int8_t axis_rz, uint32_t button_mask) {
  if (!ble_gamepad_is_connected()) {
    return;
  }

  const uint16_t buttons = static_cast<uint16_t>(button_mask & 0xFFFF);
  l3_gamepad_report_t report = {};

  report.z = axis_z;
  report.rz = axis_rz;
  report.buttons = buttons;

  blehid.inputReport(HID_GAMEPAD_REPORT_ID, &report, hid_gamepad_report_size);
}

bool ble_gamepad_is_connected(void) {
  return Bluefruit.connected() > 0;
}
