#include "solve.h"

static bool solved;

/**
 * @brief Set initial search direction for the solver.
 */
void initialize_solver_direction(void)
{
	while (1) {
		if (button_left_read_consecutive(500)) {
			set_search_initial_direction(NORTH);
			led_left_on();
			break;
		}
		if (button_right_read_consecutive(500)) {
			set_search_initial_direction(EAST);
			led_right_on();
			break;
		}
	}
	sleep_ticks(2000);
}

/**
 * @brief Solve an unknown maze.
 */
void solve(void)
{
	enum step_direction step;

	if (!solved)
		initialize_search();

	reset_motion();
	enable_motor_control();

	set_search_initial_state();
	search_update(left_wall_detection(), front_wall_detection(),
		      right_wall_detection());
	step = best_neighbor_step();
	move_search_position(step);
	set_starting_position();
	move_out();

	while (search_distance() > 0) {
		search_update(left_wall_detection(), front_wall_detection(),
			      right_wall_detection());
		step = best_neighbor_step();
		move_search_position(step);
		move(step);
		if (collision_detected())
			return;
	}
	stop_middle();
	solved = true;
	search_update(left_wall_detection(), front_wall_detection(),
		      right_wall_detection());
}
