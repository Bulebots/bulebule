#ifndef __HMI_H
#define __HMI_H

#include <libopencm3/stm32/gpio.h>

void led_left_toggle(void);
void led_right_toggle(void);
void led_left_on(void);
void led_right_on(void);
void led_left_off(void);
void led_right_off(void);

#endif /* __HMI_H */
