#ifndef __BUTTONS_H
#define __BUTTONS_H

#include <libopencm3/stm32/gpio.h>

bool button_left_read(void);
bool button_right_read(void);

#endif /* __BUTTONS_H */
