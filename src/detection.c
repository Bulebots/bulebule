#include "detection.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>

static void sm_emitter_adc(void);

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
 * @brief State machine to manage the sensors activation and deactivation
 * states and readings.
 *
 * In order to get accurate distance values, the phototransistor's output
 * will be read with the infrared emitter sensors powered on and powered
 * off.
 *
 * - State 0: Do nothing.
 * - State 1: Save ADC registers from emitter OFF and power ON the emitter.
 * - State 2: Start the ADC read.
 * - State 3: Save ADC registers from emitter ON and power OFF the emitter.
 * - State 4: Start the ADC read.
 */
static void sm_emitter_adc(void)
{
	static uint8_t emitter_status = EMIT_UNDEFINED;
	static uint16_t sensors_off[4], sensors_on[4];
	switch (emitter_status) {
	case EMIT_UNDEFINED:
		emitter_status = EMIT_ON;
		break;
	case EMIT_ON:
		sensors_off[SENSOR_SIDE_LEFT] = adc_read_injected(ADC1, 1);
		sensors_off[SENSOR_FRONT_LEFT] = adc_read_injected(ADC1, 2);
		sensors_off[SENSOR_FRONT_RIGHT] = adc_read_injected(ADC1, 3);
		sensors_off[SENSOR_SIDE_RIGHT] = adc_read_injected(ADC1, 4);
		printf("sensors_off %d\n", sensors_off[SENSOR_SIDE_LEFT]);
		gpio_toggle(GPIOA, GPIO7);
		emitter_status = EMIT_ON_ADC_ON;
		break;
	case EMIT_ON_ADC_ON:
		adc_start_conversion_injected(ADC1);
		emitter_status = EMIT_OFF;
		break;
	case EMIT_OFF:
		sensors_on[SENSOR_SIDE_LEFT] = adc_read_injected(ADC1, 1);
		sensors_on[SENSOR_FRONT_LEFT] = adc_read_injected(ADC1, 2);
		sensors_on[SENSOR_FRONT_RIGHT] = adc_read_injected(ADC1, 3);
		sensors_on[SENSOR_SIDE_RIGHT] = adc_read_injected(ADC1, 4);
		printf("sensors_on %d\n", sensors_on[SENSOR_SIDE_LEFT]);
		gpio_toggle(GPIOA, GPIO7);
		emitter_status = EMIT_OFF_ADC_ON;
		break;
	case EMIT_OFF_ADC_ON:
		adc_start_conversion_injected(ADC1);
		emitter_status = EMIT_ON;
		break;
	default:
		break;
	}
}
