#include "solve.h"

static char run_sequence[MAZE_AREA];

/**
 * @brief Move from the current position to the defined target.
 */
static void go_to_target(void)
{
	enum step_direction step;
	struct walls_around walls;

	do {
		walls = read_walls();
		search_update(walls);
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
	search_update(walls);
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
	run_sequence[i++] = 'M';
	run_sequence[i] = '\0';
}

/**
 * @brief Execute the speed run movement sequence.
 */
void run(void)
{
	int i = 0;
	int many = 0;
	char movement;

	while (true) {
		movement = run_sequence[i++];
		if (!movement)
			break;
		switch (movement) {
		case 'F':
			many = 0;
			while (true) {
				many += 1;
				if (run_sequence[i] != 'F')
					break;
				i++;
			}
			move_front_many(many);
			break;
		case 'L':
			move_left();
			break;
		case 'R':
			move_right();
			break;
		case 'M':
			stop_middle();
			break;
		default:
			break;
		}
		if (collision_detected())
			return;
	}
}
