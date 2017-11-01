#ifndef __MOTOR_H
#define __MOTOR_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "setup.h"

void drive_break(void);
void drive_off(void);
uint32_t pwm_saturation(void);
void power_left(int32_t power);
void power_right(int32_t power);

#endif /* __MOTOR_H */
