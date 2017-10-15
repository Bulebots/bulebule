#ifndef __SETUP_H
#define __SETUP_H

/** System clock frequency is set in `setup_clock` */
#define SYSCLK_FREQUENCY_HZ 72000000
#define SYSTICK_FREQUENCY_HZ 1000
#define DRIVER_PWM_PERIOD 1024

/** Encoder-related constants */
#define MILLIMETERS_PER_COUNT 0.0084189

/** ADC constants */
#define ADC_12_BITS 4096
#define V_REF_MV 3300

/** Voltage divider */
#define VOLT_DIV_FACTOR 2
/* Battery threshold:
 * - We want to stop draining the LIPO battery with a voltage of 3.6 V.
 * - The battery is connected to a voltage divider of 2: 3.6 V / 2 = 1.8 V.
 * - For 12 bits ADC, 1 LSB ideal = Vref (3,3 V)/ 4096.
 * - Low_threshold = 1.8 * 4096 / 3.3 = 2234.
 */
#define BATTERY_LOW_LIMIT 2234

#endif /* __SETUP_H */
