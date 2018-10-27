#ifndef __CLOCK_H
#define __CLOCK_H

#include <stdint.h>

#include <libopencm3/cm3/dwt.h>

#include "setup.h"

void each(uint32_t period, void (*function)(void), uint32_t during);
uint32_t get_clock_ticks(void);
uint32_t read_cycle_counter(void);
void sleep_ticks(uint32_t ticks);
void sleep_seconds(float seconds);
void stopwatch_start(void);
float stopwatch_stop(void);
void sleep_us(uint32_t us);
void sleep_us_after(uint32_t cycle_counter, uint32_t us);
void clock_tick(void);

#endif /* __CLOCK_H */
