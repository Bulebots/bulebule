#ifndef __LOGGING_H
#define __LOGGING_H

#include "battery.h"
#include "control.h"
#include "encoder.h"
#include "formatting.h"

void log_battery_voltage(void);
void log_linear_speed(void);
void log_angular_speed(void);

#endif /* __LOGGING_H */
