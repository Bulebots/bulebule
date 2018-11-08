#include "diagnostics.h"

static volatile float log_matrix[NUM_LOG_ELEMENTS];
static volatile int log_set_index = -1;
static volatile int log_get_index = -1;
static volatile int num_elements_counter;
static volatile bool enable_diagnostics;
static volatile bool enable_read_log;

static int inc_log_set_index(void)
{
	if (log_set_index == (NUM_LOG_ELEMENTS - 1)) {
		log_set_index = 0;
	} else {
		log_set_index++;
	}
	LOG_INFO("%d set_index", log_set_index);
	return log_set_index;
}

static int inc_log_get_index(void)
{
	if (log_get_index == (NUM_LOG_ELEMENTS - 1)) {
		log_get_index = 0;
	} else {
		log_get_index++;
	}
	LOG_INFO("%d get_index", log_get_index);
	return log_get_index;
}

bool get_enable_diagnostics(void)
{
	return enable_diagnostics;
}

void set_enable_diagnostics(bool value)
{
	enable_diagnostics = value;
}

int get_log_set_index(void)
{
	return log_set_index;
}

/**
 * @brief Function to set the sensors distance.
 */
void set_log_matrix(void)
{
	if (get_enable_diagnostics()) {
		log_matrix[inc_log_set_index()] = get_front_left_distance();
	}
}

/**
 * @brief Function to get the sensors distance.
 *
 *@return The log_matrix
 */
float get_log_matrix_element(void)
{
	if (num_elements_counter < NUM_LOG_ELEMENTS) {
		num_elements_counter++;
		return log_matrix[inc_log_get_index()];
	}
	return 0;
}

/**
 * @brief Function to reset the buffer.
 */
void reset_log_matrix(void)
{
	log_set_index = -1;
	log_get_index = -1;
	num_elements_counter = 0;
}
