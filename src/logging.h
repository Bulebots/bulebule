#ifndef __LOGGING_H
#define __LOGGING_H

#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include "battery.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "move.h"
#include "mpu.h"
#include "serial.h"
#include "speed.h"

enum { LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR };
static const char *const log_level_strings[] = {"DEBUG", "INFO", "WARNING",
						"ERROR"};

#define LOG_MESSAGE(level, format, arg...)                                     \
	do {                                                                   \
		uint32_t time = get_clock_ticks();                             \
		static char tx_buffer[400];                                    \
		sprintf(tx_buffer, "%" PRIu32 ",%s,%s:%d,%s," format "\n",     \
			time, log_level_strings[level], __FILE__, __LINE__,    \
			__func__, ##arg);                                      \
		dma_write(tx_buffer, strlen(tx_buffer));                       \
	} while (0)

#define LOG_DATA(format, arg...)                                               \
	do {                                                                   \
		uint32_t time = get_clock_ticks();                             \
		static char tx_buffer[100];                                    \
		sprintf(tx_buffer, "%" PRIu32 ",DATA,,," format "\n", time,    \
			##arg);                                                \
		dma_write(tx_buffer, strlen(tx_buffer));                       \
	} while (0)

#define LOG_DEBUG(format, arg...) LOG_MESSAGE(LOG_LEVEL_DEBUG, format, ##arg)
#define LOG_INFO(format, arg...) LOG_MESSAGE(LOG_LEVEL_INFO, format, ##arg)
#define LOG_WARNING(format, arg...)                                            \
	LOG_MESSAGE(LOG_LEVEL_WARNING, format, ##arg)
#define LOG_ERROR(format, arg...) LOG_MESSAGE(LOG_LEVEL_ERROR, format, ##arg)

void start_data_logging(void (*log_function)(void));
void stop_data_logging(void);
void log_data(void);
void log_data_front_sensors_calibration(void);
void log_data_control(void);
void log_battery_voltage(void);
void log_configuration_variables(void);
void log_linear_speed(void);
void log_angular_speed(void);
void log_sensors_distance(void);
void log_encoders_counts(void);
void log_sensors_raw(void);
void log_side_sensors_error(void);
void log_front_sensors_error(void);
void log_walls_detection(void);

#endif /* __LOGGING_H */
