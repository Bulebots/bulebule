#ifndef __LOGGING_H
#define __LOGGING_H

#include "battery.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "formatting.h"
#include "move.h"
#include "mpu.h"
#include "speed.h"

void log_battery_voltage(void);
void log_configuration_variables(void);
void log_linear_speed(void);
void log_angular_speed(void);
void log_sensors_distance(void);
void log_encoders_counts(void);
void log_sensors_raw(void);
void log_side_sensors_error(void);
void log_front_sensors_calibration(void);
void log_front_sensors_error(void);
void log_walls_detection(void);

#endif /* __LOGGING_H */
