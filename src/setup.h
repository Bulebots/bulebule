#ifndef __SETUP_H
#define __SETUP_H

#include <libopencm3/cm3/dwt.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

#include <mpu.h>
/** Universal constants */
#define MICROMETERS_PER_METER 1000000
#define MICROSECONDS_PER_SECOND 1000000
#define PI 3.1415
#define CELL_DIMENSION 0.18
#define WALL_WIDTH 0.012
#define MOUSE_TAIL 0.037
#define MOUSE_HEAD 0.057
#define MOUSE_AXIS_SEPARATION 0.0223
#define MIDDLE_MAZE_DISTANCE ((CELL_DIMENSION - WALL_WIDTH) / 2.)

/** System clock frequency is set in `setup_clock` */
#define SYSCLK_FREQUENCY_HZ 72000000
#define SYSTICK_FREQUENCY_HZ 1000
#define DRIVER_PWM_PERIOD 1024

/**
 * Maximum PWM period (should be <= DRIVER_PWM_PERIOD).
 *
 * Usually it should be set to DRIVER_PWM_PERIOD except for testing purposes
 * in order to protect the locomotion system and avoid reaching uncontrolled
 * speeds.
 */
#define MAX_PWM_PERIOD 1024

/**
 * Maximum time period allowed with saturated PWM output.
 *
 * After reaching this period we consider there has been a collision. When a
 * collision occurs, the robot motor control stops working and the motor driver
 * is disabled.
 */
#define MAX_PWM_SATURATION_PERIOD 0.01

/** ADC constants */
#define ADC_RESOLUTION 4096
#define ADC_LSB (3.3 / ADC_RESOLUTION)

/** Voltage divider */
#define VOLT_DIV_FACTOR 2
#define BATTERY_LOW_LIMIT_VOLTAGE 3.3

void setup(void);
void setup_spi_low_speed(void);
void setup_spi_high_speed(void);

#endif /* __SETUP_H */
