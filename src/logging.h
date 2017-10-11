#ifndef __LOGGING_H
#define __LOGGING_H

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <libopencm3/stm32/usart.h>

#include "clock.h"

enum { LOG_LEVEL_INFO, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR };
static const char *const log_level_strings[] = {"INFO", "WARNING", "ERROR"};

int _write(int file, char *ptr, int len);

#define LOG_MESSAGE(level, format, arg...)                                     \
	do {                                                                   \
		uint32_t time = get_clock_ticks();                             \
		printf("%ld,%s,%s:%d: " format "\n", time,                     \
		       log_level_strings[level], __FILE__, __LINE__, ##arg);   \
	} while (0)

#define LOG_INFO(format, arg...) LOG_MESSAGE(LOG_LEVEL_INFO, format, ##arg)
#define LOG_WARNING(format, arg...)                                            \
	LOG_MESSAGE(LOG_LEVEL_WARNING, format, ##arg)
#define LOG_ERROR(format, arg...) LOG_MESSAGE(LOG_LEVEL_ERROR, format, ##arg)

#endif /* __LOGGING_H */
