#ifndef __COMMON_H
#define __COMMON_H

#include <libopencm3/stm32/adc.h>

bool adc_get_flag(uint32_t adc_peripheral, uint32_t flag);
void adc_clear_flag(uint32_t adc_peripheral, uint32_t flag);
int sign(float number);

#endif /* __COMMON_H */
