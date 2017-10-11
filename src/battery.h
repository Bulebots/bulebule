#ifndef __BATTERY_H
#define __BATTERY_H

#include "logging.h"
#include "setup.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>

void adc1_2_isr(void);
float get_battery_level(void);

#endif /* __BATTERY_H */
