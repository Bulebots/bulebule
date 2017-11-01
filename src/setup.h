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

/**
 * Maximum PWM period (should be <= DRIVER_PWM_PERIOD).
 *
 * Usually it should be set to DRIVER_PWM_PERIOD except for testing purposes
 * in order to protect the locomotion system and avoid reaching uncontrolled
 * speeds.
 */
#define MAX_PWM_PERIOD 300

/**
 * Maximum time period allowed with saturated PWM output.
 *
 * After reaching this period we consider there has been a collision. When a
 * collision occurs, the robot motor control stops working and the motor driver
 * is disabled.
 */
#define MAX_PWM_SATURATION_PERIOD 0.01

/** Unit conversion */
#define MICROMETERS_PER_METER 1000000

/** ADC constants */
#define ADC_LSB (3.3 / 4096)

/** Voltage divider */
#define VOLT_DIV_FACTOR 2
#define BATTERY_LOW_LIMIT_VOLTAGE 3.3

void setup(void);

#endif /* __SETUP_H */
