#include "solve.h"

/**
 * @brief Solve an unknown maze.
 */
void solve_search(void)
{
	enum step_direction step;

	initialize_search();

	search_update(left_wall_detection(), front_wall_detection(),
		      right_wall_detection());
	step = best_neighbor_step();
	move_search_position(step);
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
}
