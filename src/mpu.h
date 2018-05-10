#ifndef __MPU_H
#define __MPU_H

#include <stdint.h>

#include "clock.h"
#include "setup.h"

uint8_t mpu_who_am_i(void);

void setup_mpu(void);
int16_t get_gyro_z_raw(void);
float get_gyro_z_dps(void);
void gyro_z_calibration(void);
float get_gyro_z_degrees(void);
void update_gyro_readings(void);
float get_gyro_z_radps(void);

#endif /* __MPU_H */
