#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include "clock.h"
#include "control.h"
#include "detection.h"
#include "logging.h"

void run_linear_speed_profile(void);
void run_angular_speed_profile(void);
void run_distances_profiling(void);

#endif /* __CALIBRATION_H */
