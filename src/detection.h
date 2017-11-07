#ifndef __DETECTION_H
#define __DETECTION_H

#include <math.h>
#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "setup.h"

/* Sensors IDs*/
#define SENSOR_SIDE_LEFT_ID 0
#define SENSOR_SIDE_RIGHT_ID 1
#define SENSOR_FRONT_LEFT_ID 2
#define SENSOR_FRONT_RIGHT_ID 3
#define NUM_SENSOR 4

struct sides_info {
	bool left : 1;
	bool right : 1;
};

void get_gyro_raw(uint16_t *vo, uint16_t *vref);
void get_sensors_raw(uint16_t *off, uint16_t *on);
void update_distance_readings(void);
float get_side_left_distance(void);
float get_side_right_distance(void);
float get_front_left_distance(void);
float get_front_right_distance(void);
float get_side_sensors_error(void);
float get_front_sensors_error(void);
bool front_wall_detection(void);
bool right_wall_detection(void);
bool left_wall_detection(void);

#endif /* __DETECTION_H */
