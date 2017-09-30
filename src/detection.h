#ifndef __DETECTION_H
#define __DETECTION_H

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>

void tim1_up_isr(void);

/* State machine status */
#define EMIT_UNDEFINED 0
#define EMIT_ON 1
#define EMIT_ON_ADC_ON 2
#define EMIT_OFF 3
#define EMIT_OFF_ADC_ON 4

/* Sensors */
#define SENSOR_SIDE_LEFT 0
#define SENSOR_SIDE_RIGHT 1
#define SENSOR_FRONT_LEFT 2
#define SENSOR_FRONT_RIGHT 3

#endif /* __DETECTION_H */
