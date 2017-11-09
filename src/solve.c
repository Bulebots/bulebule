#include "solve.h"

static bool solved;

/**
 * @brief Solve an unknown maze.
 */
void solve(void)
{
	enum step_direction step;

	if (!solved)
		initialize_search();

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
	}
	search_update(left_wall_detection(), front_wall_detection(),
		      right_wall_detection());
	stop_middle();
	solved = true;
}
