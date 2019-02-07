#ifndef __CONFIG_H
#define __CONFIG_H

/** Locomotion-related constants */
#define MICROMETERS_PER_COUNT 8.32
#define WHEELS_SEPARATION 0.0785
#define SHIFT_AFTER_180_DEG_TURN 0.010

/** Time it takes for the robot to decide where to go next while searching */
#define SEARCH_REACTION_TIME 0.01

/** Calibration constants for sensors */
#define SENSOR_SIDE_LEFT_A 2.806
#define SENSOR_SIDE_LEFT_B 0.287
#define SENSOR_SIDE_RIGHT_A 2.327
#define SENSOR_SIDE_RIGHT_B 0.231
#define SENSOR_FRONT_LEFT_A 2.609
#define SENSOR_FRONT_LEFT_B 0.242
#define SENSOR_FRONT_RIGHT_A 2.713
#define SENSOR_FRONT_RIGHT_B 0.258

#endif /* _CONFIG_H */
