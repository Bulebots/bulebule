#ifndef __SETUP_H
#define __SETUP_H

/** System clock frequency is set in `setup_clock` */
#define SYSCLK_FREQUENCY_HZ 72000000
#define SYSTICK_FREQUENCY_HZ 1000
#define DRIVER_PWM_PERIOD 1024

/** ADC constants **/
#define ADC_12_BITS 4096
#define V_REF_MV 3300

/** Voltage divider **/
#define VOLT_DIV_FACTOR 2

#endif /* __SETUP_H */
