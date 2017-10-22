#ifndef __SETUP_H
#define __SETUP_H

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

/** System clock frequency is set in `setup_clock` */
#define SYSCLK_FREQUENCY_HZ 72000000
#define SYSTICK_FREQUENCY_HZ 1000
#define DRIVER_PWM_PERIOD 1024

/** Encoder-related constants */
#define MICROMETERS_PER_COUNT 8.4189

/** Maximum acceleration and deceleration in meters per second squared */
#define MAX_ACCELERATION 1.
#define MAX_DECELERATION 1.

/** Unit conversion */
#define MICROMETERS_PER_METER 1000000

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

void setup(void);

#endif /* __SETUP_H */
