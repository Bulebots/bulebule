#ifndef __VOLTAGE_H
#define __VOLTAGE_H

#include <libopencm3/stm32/adc.h>

#include "setup.h"

float get_battery_voltage(void);
float get_motor_driver_input_voltage(void);

#endif /* __VOLTAGE_H */
