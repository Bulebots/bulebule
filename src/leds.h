#ifndef __LEDS_H
#define __LEDS_H

#include <libopencm3/stm32/gpio.h>

void led_left_toggle(void);
void led_right_toggle(void);
void led_bluepill_toggle(void);
void led_left_on(void);
void led_right_on(void);
void led_bluepill_on(void);
void led_left_off(void);
void led_right_off(void);
void led_bluepill_off(void);

#endif /* __LEDS_H */
