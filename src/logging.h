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

#define LOG_MESSAGE_TIMEOUT 10

enum { LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR };
static const char *const log_level_strings[] = {"DEBUG", "INFO", "WARNING",
						"ERROR"};

/**
 * @brief Log a message with a provided level and format.
 *
 * The `format` string is formatted with the passed extra parameters. Some
 * other parameters are prepended to that string, in CSV-formatted style:
 *
 * - Time (in clock ticks)
 * - Log level
 * - File and line where the macro was called
 * - Function from where the macro was called
 *
 * Log messages always end with a `\n` character.
 *
 * This function waits for `LOG_MESSAGE_TIMEOUT` for the serial interface to
 * be available before writing the log message. After the timeout occurs, it
 * will always write the message to the DMA buffer anyways.
 */
#define LOG_MESSAGE(level, format, arg...)                                     \
	do {                                                                   \
		uint32_t time = get_clock_ticks();                             \
		static char tx_buffer[400];                                    \
		sprintf(tx_buffer, "%" PRIu32 ",%s,%s:%d,%s," format "\n",     \
			time, log_level_strings[level], __FILE__, __LINE__,    \
			__func__, ##arg);                                      \
		serial_wait_send_available(LOG_MESSAGE_TIMEOUT);               \
		serial_send(tx_buffer, strlen(tx_buffer));                     \
	} while (0)

/**
 * @brief Log some data with a provided format.
 *
 * The `format` string is formatted with the passed extra parameters. Some
 * other parameters are prepended to that string, in CSV-formatted style:
 *
 * - Time (in clock ticks)
 * - Log level, which is always "DATA"
 * - File and line where the macro was called is omitted
 * - Function from where the macro was called is omitted
 *
 * Data logs always end with a `\n` character.
 *
 * This macro checks if the serial interface is not busy sending data. If it is
 * it will simply do nothing and discard the data log.
 */
#define LOG_DATA(format, arg...)                                               \
	do {                                                                   \
		uint32_t time = get_clock_ticks();                             \
		static char tx_buffer[100];                                    \
		if (!serial_transfer_complete())                               \
			break;                                                 \
		sprintf(tx_buffer, "%" PRIu32 ",DATA,,," format "\n", time,    \
			##arg);                                                \
		serial_send(tx_buffer, strlen(tx_buffer));                     \
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
