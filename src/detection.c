#include "detection.h"

static void sm_emitter_adc(void);

/**
 * @brief State machine to manage the sensors activation and deactivation
 * states and readings.
 *
 * In order to get accurate distance values, the phototransistor's output
 * will be read with the infrared emitter sensors powered on and powered
 * off.
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
	static uint16_t sensors_off[4], sensors_on[4];
	static int32_t gyro_deg_raw;

	switch (emitter_status) {
	case 1:
		adc_start_conversion_injected(ADC2);
		sensors_off[SENSOR_SIDE_LEFT] = adc_read_injected(ADC1, 1);
		sensors_off[SENSOR_SIDE_RIGHT] = adc_read_injected(ADC1, 2);
		sensors_off[SENSOR_FRONT_LEFT] = adc_read_injected(ADC1, 3);
		sensors_off[SENSOR_FRONT_RIGHT] = adc_read_injected(ADC1, 4);
		gpio_toggle(GPIOA, GPIO7);
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
		sensors_on[SENSOR_SIDE_LEFT] = adc_read_injected(ADC1, 1);
		sensors_on[SENSOR_SIDE_RIGHT] = adc_read_injected(ADC1, 2);
		sensors_on[SENSOR_FRONT_LEFT] = adc_read_injected(ADC1, 3);
		sensors_on[SENSOR_FRONT_RIGHT] = adc_read_injected(ADC1, 4);
		gpio_toggle(GPIOA, GPIO7);
		emitter_status = 4;
		break;
	case 4:
		adc_start_conversion_injected(ADC1);
		gyro_deg_raw =
		    adc_read_injected(ADC2, 1) - adc_read_injected(ADC2, 2);
		emitter_status = 1;
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
 * @brief Function to get data from gyroscope.
 *
 * - Return the output and reference voltages of gyroscope on bits.
 */
void get_gyro_raw(uint16_t *vo, uint16_t *vref)
{
	*vo = adc_read_injected(ADC2, 1);
	*vref = adc_read_injected(ADC2, 2);
}
