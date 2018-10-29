#include "solve.h"

#define EEPROM_NUM_BYTES_ERASED_CHECKED ((uint8_t)4)
#define EEPROM_BYTE_ERASED_VALUE 255
static char run_sequence[MAZE_AREA];
static enum movement smooth_path[MAZE_AREA];

/**
 * @brief Move from the current position to the defined target.
 */
static void go_to_target(void)
{
	enum step_direction step;
	struct walls_around walls;

	set_distances();
	do {
		if (!current_cell_is_visited()) {
			walls = read_walls();
			update_walls(walls);
			set_distances();
		} else {
			walls = current_walls_around();
		}
#ifdef MMSIM_SIMULATION
		send_state();
#endif
		step = best_neighbor_step(walls);
		move_search_position(step);
		move(step);
		if (collision_detected())
			return;
	} while (search_distance() > 0);

	walls = read_walls();
	update_walls(walls);
}

/**
 * @brief Execute the maze exploration.
 *
 * After reaching the goal, it will try to explore remaining parts until
 * finding an optimal path.
 */
void explore(void)
{
	uint8_t cell;

	initialize_maze_walls();
	set_search_initial_state();

	while (true) {
		go_to_target();
		if (collision_detected())
			return;
		if (search_position() == 0)
			break;
		cell = find_unexplored_interesting_cell();
		set_target_cell(cell);
	}
	stop_middle();
}

/**
 * @brief Define the movement sequence to be executed on speed runs.
 */
void set_run_sequence(void)
{
	int i = 0;
	enum step_direction step;

	set_search_initial_state();
	set_target_goal();
	set_distances();
	while (search_distance() > 0) {
		step = best_neighbor_step(current_walls_around());
		switch (step) {
		case FRONT:
			run_sequence[i++] = 'F';
			break;
		case LEFT:
			run_sequence[i++] = 'L';
			break;
		case RIGHT:
			run_sequence[i++] = 'R';
			break;
		default:
			break;
		}
		move_search_position(step);
	}
	while (best_neighbor_step(current_walls_around()) == FRONT) {
		move_search_position(FRONT);
		run_sequence[i++] = 'F';
		if (search_distance() != 0)
			break;
	}
	run_sequence[i++] = 'S';
	run_sequence[i] = '\0';
}

/**
 * @brief Execute the speed run movement sequence.
 *
 * @param[in] speed The speed level at which to execute the run sequence.
 */
void run(uint8_t speed)
{
	int i = 0;
	int many = 0;
	char movement;
	float distance = 0;
	bool begin = true;

	make_smooth_path(run_sequence, smooth_path);
	while (true) {
		movement = smooth_path[i++];
		if (movement == MOVE_END)
			break;
		switch (movement) {
		case MOVE_FRONT:
			many = 0;
			while (true) {
				many += 1;
				if (smooth_path[i] != MOVE_FRONT)
					break;
				i++;
			}
			distance += many * CELL_DIMENSION;
			if (begin) {
				distance -= (MOUSE_TAIL + WALL_WIDTH / 2 +
					     MOUSE_AXIS_SEPARATION / 2);
				begin = false;
			}
			break;
		case MOVE_LEFT:
		case MOVE_RIGHT:
		case MOVE_LEFT_90:
		case MOVE_RIGHT_90:
		case MOVE_LEFT_180:
		case MOVE_RIGHT_180:
			distance += get_move_turn_space(movement, speed);
			parametric_move_front(
			    distance,
			    get_move_turn_linear_speed(movement, speed));
			speed_turn(movement, speed);
			distance = get_move_turn_space(movement, speed);
			break;
		case MOVE_STOP:
			distance += CELL_DIMENSION / 2;
			parametric_move_front(distance, 0.);
			break;
		default:
			LOG_ERROR("Unable to process command [%d]!", movement);
			return;
		}
		if (collision_detected())
			return;
	}
}

/**
 * @brief Function to save the maze sequence on EEPROM.
 */
void save_maze(void)
{
	uint32_t save_status = 0;

	save_status = eeprom_flash_page(FLASH_EEPROM_ADDRESS_MAZE,
					(uint8_t *)run_sequence, MAZE_AREA);

	if (save_status != RESULT_OK)
		LOG_ERROR("EEPROM save error %" PRIu32, save_status);
}

/**
 * @brief Function to load the maze sequence from EEPROM to static on RAM.
 */
void load_maze(void)
{
	eeprom_read_data(FLASH_EEPROM_ADDRESS_MAZE, MAZE_AREA,
			 (uint8_t *)run_sequence);
}

/**
 * @brief Function to reset the maze sequence on EEPROM.
 */
void reset_maze(void)
{
	uint32_t erase_status = 0;

	erase_status = eeprom_erase_page(FLASH_EEPROM_ADDRESS_MAZE);
	if (erase_status != RESULT_OK)
		LOG_ERROR("EEPROM reset error %" PRIu32, erase_status);
}

/**
 * @brief Function to check if the maze sequence is saved on EEPROM.
 *
 *@return bool
 */
bool maze_is_saved(void)
{
	uint8_t maze_sample[EEPROM_NUM_BYTES_ERASED_CHECKED];

	eeprom_read_data(FLASH_EEPROM_ADDRESS_MAZE,
			 EEPROM_NUM_BYTES_ERASED_CHECKED, maze_sample);

	for (uint8_t iter = 0; iter < EEPROM_NUM_BYTES_ERASED_CHECKED; iter++) {
		if (maze_sample[iter] != EEPROM_BYTE_ERASED_VALUE)
			return true;
	}

	return false;
}
