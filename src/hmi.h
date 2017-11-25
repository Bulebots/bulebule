#ifndef __HMI_H
#define __HMI_H

#include <libopencm3/stm32/gpio.h>

#include "clock.h"
#include "detection.h"

void led_left_toggle(void);
void led_right_toggle(void);
void led_bluepill_toggle(void);
void led_left_on(void);
void led_right_on(void);
void led_bluepill_on(void);
void led_left_off(void);
void led_right_off(void);
void led_bluepill_off(void);
void blink_burst(void);
bool button_left_read(void);
bool button_right_read(void);
bool button_left_read_consecutive(uint32_t count);
bool button_right_read_consecutive(uint32_t count);
void wait_front_sensor_close_signal(float close_distance);

#endif /* __HMI_H */
