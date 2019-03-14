#ifndef __HMI_H
#define __HMI_H

#include <libopencm3/stm32/gpio.h>

#include "mmlib/clock.h"
#include "mmlib/solve.h"
#include "mmlib/speaker.h"
#include "mmlib/speed.h"

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
void repeat_blink(uint8_t count, uint16_t time);
void blink_collision(void);
void speaker_warn_low_battery(void);
void speaker_play_error(void);
void speaker_play_success(void);
void speaker_play_button(void);
void speaker_play_competition(void);
bool button_left_read(void);
bool button_right_read(void);
bool button_left_read_consecutive(uint32_t count);
bool button_right_read_consecutive(uint32_t count);
void wait_front_sensor_close_signal(float close_distance);
void initialize_solver_direction(void);
bool reuse_maze(void);
float hmi_configure_force(float minimum_force, float force_step);

#endif /* __HMI_H */
