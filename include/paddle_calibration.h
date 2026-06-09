#pragma once

// Per-unit pot end stops (12-bit SAADC, typically ~200–3800). Edit for each
// paddle/combo variant, or override at build: -DPOT_ADC_MIN=420 -DPOT_ADC_MAX=3680
//
// Calibration: flash with -DPOT_DEBUG=1, sweep paddle to both stops, copy the
// reported "sweep min" and "sweep max" raw values here — NOT joy.cpl axis (-127..127).

#if DEVICE_HAS_POT
#if !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 4
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 5
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 6
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 7
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 8
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 9
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 10
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#elif !defined(POT_ADC_MIN) && DEVICE_VARIANT_ID == 11
#define POT_ADC_MIN 0
#define POT_ADC_MAX 4095
#endif

#endif  // DEVICE_HAS_POT
