#ifndef __MOTOR_H
#define __MOTOR_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "setup.h"

void drive_break(void);
void drive_off(void);
void power_left(int32_t power);
void power_right(int32_t power);
uint32_t motor_driver_saturation(void);
void reset_motor_driver_saturation(void);

#endif /* __MOTOR_H */
