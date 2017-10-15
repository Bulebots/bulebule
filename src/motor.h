#ifndef __MOTOR_H
#define __MOTOR_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

void power_left(uint32_t power);
void power_right(uint32_t power);
void drive_forward(void);
void drive_backward(void);
void drive_break(void);

#endif /* __MOTOR_H */
