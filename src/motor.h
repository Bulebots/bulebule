#ifndef __MOTOR_H
#define __MOTOR_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "setup.h"

void power_left(int32_t power);
void power_right(int32_t power);
void drive_break(void);

#endif /* __MOTOR_H */
