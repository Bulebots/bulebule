#ifndef __SETUP_H
#define __SETUP_H

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

/** Universal constants */
#define MICROMETERS_PER_METER 1000000
#define PI 3.1415
#define CELL_DIMENSION 0.18

/** System clock frequency is set in `setup_clock` */
#define SYSCLK_FREQUENCY_HZ 72000000
#define SYSTICK_FREQUENCY_HZ 1000
#define DRIVER_PWM_PERIOD 1024

/** Encoder-related constants */
#define MICROMETERS_PER_COUNT 8.4189
#define WHEELS_SEPARATION 0.079

/**
 * Maximum acceleration and deceleration.
 *
 * - Linear acceleration is defined in meters per second squared.
 * - Angular acceleration is defined in radians per second squared.
 */
#define MAX_LINEAR_ACCELERATION 3.
#define MAX_LINEAR_DECELERATION 3.
#define MAX_ANGULAR_ACCELERATION (32. * PI)
#define MAX_ANGULAR_DECELERATION (32. * PI)

/**
 * Maximum PWM period (should be <= DRIVER_PWM_PERIOD).
 *
 * Usually it should be set to DRIVER_PWM_PERIOD except for testing purposes
 * in order to protect the locomotion system and avoid reaching uncontrolled
 * speeds.
 */
#define MAX_PWM_PERIOD 512

/**
 * Maximum time period allowed with saturated PWM output.
 *
 * After reaching this period we consider there has been a collision. When a
 * collision occurs, the robot motor control stops working and the motor driver
 * is disabled.
 */
#define MAX_PWM_SATURATION_PERIOD 0.01

/** ADC constants */
#define ADC_LSB (3.3 / 4096)

/** Voltage divider */
#define VOLT_DIV_FACTOR 2
#define BATTERY_LOW_LIMIT_VOLTAGE 3.3

void setup(void);

#endif /* __SETUP_H */
