#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include <inttypes.h>

#include "mmlib/clock.h"
#include "mmlib/control.h"
#include "mmlib/logging.h"
#include "mmlib/mpu.h"
#include "mmlib/speed.h"

#include "detection.h"
#include "move.h"
#include "setup.h"

void calibrate(void);
void run_linear_speed_profile(void);
void run_angular_speed_profile(void);
void run_distances_profiling(void);
void run_movement_sequence(const char *sequence);
void run_static_turn_right_profile(void);
void run_front_sensors_calibration(void);

#endif /* __CALIBRATION_H */
