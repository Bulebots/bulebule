#include "solve.h"

static void go_to_target(void) {
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

void explore(void)
{
	uint8_t cell;

	while (true) {
		go_to_target();
		if (collision_detected())
			return;
		if (search_position() == 0)
			break;
		cell = find_unexplored_interesting_cell();
		set_target_cell(cell);
	}
}
