#include "detection.h"

static volatile uint16_t sensors_off[4], sensors_on[4];
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
	case SENSOR_SIDE_LEFT:
		gpio_set(GPIOA, GPIO9);
		break;
	case SENSOR_SIDE_RIGHT:
		gpio_set(GPIOB, GPIO8);
		break;
	case SENSOR_FRONT_LEFT:
		gpio_set(GPIOA, GPIO8);
		break;
	case SENSOR_FRONT_RIGHT:
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
	case SENSOR_SIDE_LEFT:
		gpio_clear(GPIOA, GPIO9);
		break;
	case SENSOR_SIDE_RIGHT:
		gpio_clear(GPIOB, GPIO8);
		break;
	case SENSOR_FRONT_LEFT:
		gpio_clear(GPIOA, GPIO8);
		break;
	case SENSOR_FRONT_RIGHT:
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
	static uint8_t sensor_index = SENSOR_SIDE_LEFT;

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
void get_sensors_data(uint16_t *off, uint16_t *on)
{
	uint8_t i = 0;

	for (i = 0; i < NUM_SENSOR; i++) {
		off[i] = sensors_off[i];
		on[i] = sensors_on[i];
	}
}
