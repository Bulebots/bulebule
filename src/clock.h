#ifndef __CLOCK_H
#define __CLOCK_H

#include <stdint.h>

uint32_t get_clock_ticks(void);
void sleep_ticks(uint32_t ticks);
void clock_tick(void);

#endif /* __CLOCK_H */
