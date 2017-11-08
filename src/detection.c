#include "detection.h"

#define SIDE_WALL_DETECTION (CELL_DIMENSION * 0.75)
#define FRONT_WALL_DETECTION (CELL_DIMENSION * 1.25)

static volatile uint16_t sensors_off[NUM_SENSOR], sensors_on[NUM_SENSOR];
static volatile float distance[NUM_SENSOR];
const float sensors_calibration_a[NUM_SENSOR] = {
    SENSOR_SIDE_LEFT_A, SENSOR_SIDE_RIGHT_A, SENSOR_FRONT_LEFT_A,
    SENSOR_FRONT_RIGHT_A};
const float sensors_calibration_b[NUM_SENSOR] = {
    SENSOR_SIDE_LEFT_B, SENSOR_SIDE_RIGHT_B, SENSOR_FRONT_LEFT_B,
    SENSOR_FRONT_RIGHT_B};

static void sm_emitter_adc(void);
static void set_emitter_on(uint8_t emitter);
static void set_emitter_off(uint8_t emitter);

/**
 * @brief Set an specific emitter ON.
 *
 * @param[in] emitter Emitter type.
 */
static void set_emitter_on(uint8_t emitter)
{
	switch (emitter) {
	case SENSOR_SIDE_LEFT_ID:
		gpio_set(GPIOA, GPIO9);
		break;
	case SENSOR_SIDE_RIGHT_ID:
		gpio_set(GPIOB, GPIO8);
		break;
	case SENSOR_FRONT_LEFT_ID:
		gpio_set(GPIOA, GPIO8);
		break;
	case SENSOR_FRONT_RIGHT_ID:
		gpio_set(GPIOB, GPIO9);
		break;
	default:
		break;
	}
}

/**
 * @brief Set an specific emitter OFF.
 *
 * @param[in] emitter Emitter type.
 */
static void set_emitter_off(uint8_t emitter)
{
	switch (emitter) {
	case SENSOR_SIDE_LEFT_ID:
		gpio_clear(GPIOA, GPIO9);
		break;
	case SENSOR_SIDE_RIGHT_ID:
		gpio_clear(GPIOB, GPIO8);
		break;
	case SENSOR_FRONT_LEFT_ID:
		gpio_clear(GPIOA, GPIO8);
		break;
	case SENSOR_FRONT_RIGHT_ID:
		gpio_clear(GPIOB, GPIO9);
		break;
	default:
		break;
	}
}

/**
 * @brief State machine to manage the sensors activation and deactivation
 * states and readings.
 *
 * In order to get accurate distance values, the phototransistor's output
 * will be read with the infrared emitter sensors powered on and powered
 * off. Besides, to avoid undesired interactions between different emitters and
 * phototranistors, the reads will be done one by one.
 *
 * The gyroscope uses interleaved states to give some time to ADC
 * to convert channels.
 *
 * - State 1 (first because the emitter is OFF on start):
 *         -# Start the gyroscope (ADC2) read.
 *         -# Save phototranistors sensors (ADC1) from emitter OFF and
 *            power ON the emitter.
 * - State 2:
 *         -# Start the phototranistors sensors (ADC1) read.
 *         -# Sum and save the difference of output and reference from
	      gyroscope.
 * - State 3:
 *         -# Start the gyroscope (ADC2) read.
 *         -# Save phototranistors sensors (ADC1) from emitter ON and
 *            power OFF the emitter.
 * - State 4:
 *         -# Start the phototranistors sensors (ADC1) read.
 *         -# Sum and save the difference of output and reference from
 *            gyroscope.
 */
static void sm_emitter_adc(void)
{
	static uint8_t emitter_status = 1;
	static int32_t gyro_deg_raw;
	static uint8_t sensor_index = SENSOR_SIDE_LEFT_ID;

	switch (emitter_status) {
	case 1:
		adc_start_conversion_injected(ADC2);
		sensors_off[sensor_index] =
		    adc_read_injected(ADC1, (sensor_index + 1));
		set_emitter_on(sensor_index);
		emitter_status = 2;
		break;
	case 2:
		adc_start_conversion_injected(ADC1);
		gyro_deg_raw =
		    adc_read_injected(ADC2, 1) - adc_read_injected(ADC2, 2);
		emitter_status = 3;
		break;
	case 3:
		adc_start_conversion_injected(ADC2);
		sensors_on[sensor_index] =
		    adc_read_injected(ADC1, (sensor_index + 1));
		set_emitter_off(sensor_index);
		emitter_status = 4;
		break;
	case 4:
		adc_start_conversion_injected(ADC1);
		gyro_deg_raw =
		    adc_read_injected(ADC2, 1) - adc_read_injected(ADC2, 2);
		emitter_status = 1;
		if (sensor_index == (NUM_SENSOR - 1))
			sensor_index = 0;
		else
			sensor_index++;
		break;
	default:
		break;
	}
}

/**
 * @brief TIM1 interruption routine.
 *
 * - Manage the update event interruption flag.
 * - Trigger state machine to manage sensors.
 */
void tim1_up_isr(void)
{
	if (timer_get_flag(TIM1, TIM_SR_UIF)) {
		timer_clear_flag(TIM1, TIM_SR_UIF);
		sm_emitter_adc();
	}
}

/**
 * @brief Get output and references voltages from gyroscope on bits.
 */
void get_gyro_raw(uint16_t *vo, uint16_t *vref)
{
	*vo = adc_read_injected(ADC2, 1);
	*vref = adc_read_injected(ADC2, 2);
}

/**
 * @brief Get sensors values with emitter on and off.
 */
void get_sensors_raw(uint16_t *off, uint16_t *on)
{
	uint8_t i = 0;

	for (i = 0; i < NUM_SENSOR; i++) {
		off[i] = sensors_off[i];
		on[i] = sensors_on[i];
	}
}

/**
 * @brief Calculate and update the distance from each sensor.
 *
 * @note The distances are calculated from the center of the robot.
 */
void update_distance_readings(void)
{
	uint8_t i = 0;

	for (i = 0; i < NUM_SENSOR; i++) {
		distance[i] =
		    (sensors_calibration_a[i] /
			 log((float)(sensors_on[i] - sensors_off[i])) -
		     sensors_calibration_b[i]);
	}
}

/**
 * @brief Get distance value from front left sensor.
 */
float get_front_left_distance(void)
{
	return distance[SENSOR_FRONT_LEFT_ID];
}

/**
 * @brief Get distance value from front right sensor.
 */
float get_front_right_distance(void)
{
	return distance[SENSOR_FRONT_RIGHT_ID];
}

/**
 * @brief Get distance value from side left sensor.
 */
float get_side_left_distance(void)
{
	return distance[SENSOR_SIDE_LEFT_ID];
}

/**
 * @brief Get distance value from side right sensor.
 */
float get_side_right_distance(void)
{
	return distance[SENSOR_SIDE_RIGHT_ID];
}

/**
 * @brief Calculate and return the side sensors error.
 *
 * Taking into account that the walls are parallel to the robot, this function
 * returns the distance that the robot is moved from the center of the
 * corridor..
 */
float get_side_sensors_error(void)
{
	float sensors_error;

	if ((distance[SENSOR_SIDE_LEFT_ID] > MIDDLE_MAZE_DISTANCE) &&
	    (distance[SENSOR_SIDE_RIGHT_ID] < MIDDLE_MAZE_DISTANCE)) {
		sensors_error =
		    distance[SENSOR_SIDE_RIGHT_ID] - MIDDLE_MAZE_DISTANCE;
	} else if ((distance[SENSOR_SIDE_RIGHT_ID] > MIDDLE_MAZE_DISTANCE) &&
		   (distance[SENSOR_SIDE_LEFT_ID] < MIDDLE_MAZE_DISTANCE)) {
		sensors_error =
		    MIDDLE_MAZE_DISTANCE - distance[SENSOR_SIDE_LEFT_ID];
	} else {
		sensors_error = 0;
	}
	return sensors_error;
}

/**
 * @brief Calculate and return the front sensors error.
 *
 * Taking into account that robot is approaching to a perpendicular wall, this
 * function returns the difference between the front sensors distances
 */
float get_front_sensors_error(void)
{
	return distance[SENSOR_FRONT_LEFT_ID] - distance[SENSOR_FRONT_RIGHT_ID];
}

/**
 * @brief Detect the existance or absence of the left wall.
 */
bool left_wall_detection(void)
{
	return (distance[SENSOR_SIDE_LEFT_ID] < SIDE_WALL_DETECTION) ? true
								     : false;
}

/**
 * @brief Detect the existance or absence of the right wall.
 */
bool right_wall_detection(void)
{
	return (distance[SENSOR_SIDE_RIGHT_ID] < SIDE_WALL_DETECTION) ? true
								      : false;
}

/**
 * @brief Detect the existance or absence of the front wall.
 */
bool front_wall_detection(void)
{
	return ((distance[SENSOR_FRONT_LEFT_ID] < FRONT_WALL_DETECTION) &&
		(distance[SENSOR_FRONT_RIGHT_ID] < FRONT_WALL_DETECTION))
		   ? true
		   : false;
}
