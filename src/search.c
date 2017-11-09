#include "search.h"

uint8_t distances[MAZE_SIZE * MAZE_SIZE];
uint8_t walls[MAZE_SIZE * MAZE_SIZE];

enum compass_direction initial_direction = NORTH;

uint8_t current_position;
enum compass_direction current_direction;
struct sensors_detection {
	bool front : 1;
	bool left : 1;
	bool right : 1;
} current_walls;

struct data_stack {
	uint8_t data[2 * MAZE_AREA];
	uint32_t size;
} stack;

void *requester;

/**
 * @brief Push a cell to the stack.
 */
static void push_cell(uint8_t cell)
{
	stack.data[stack.size++] = cell;
}

/**
 * @brief Pop a cell from the stack.
 */
static uint8_t pop_cell(void)
{
	return stack.data[--stack.size];
}

static void set_current_walls(bool left, bool front, bool right)
{
	current_walls.left = left;
	current_walls.front = front;
	current_walls.right = right;
}

void set_search_initial_direction(enum compass_direction direction)
{
	initial_direction = direction;
}

void set_search_initial_state(void)
{
	current_position = 0;
	current_direction = initial_direction;
	current_walls.front = 0;
	current_walls.left = 1;
	current_walls.right = 1;
}

static enum compass_direction next_compass_direction(enum step_direction step)
{
	if (step == LEFT) {
		if (current_direction == EAST)
			return NORTH;
		if (current_direction == SOUTH)
			return EAST;
		if (current_direction == WEST)
			return SOUTH;
		return WEST;
	}
	if (step == RIGHT) {
		if (current_direction == EAST)
			return SOUTH;
		if (current_direction == SOUTH)
			return WEST;
		if (current_direction == WEST)
			return NORTH;
		return EAST;
	}
	if (step == FRONT)
		return current_direction;
	return -current_direction;
}

/**
 * @brief Return the position after a given step.
 */
static uint8_t next_step_position(enum step_direction step)
{
	return current_position + next_compass_direction(step);
}

static bool wall_exists(uint8_t position, uint8_t bit)
{
	return (walls[position] & bit);
}

static void build_wall(uint8_t position, uint8_t bit)
{
	walls[position] |= bit;
	switch (bit) {
	case EAST_BIT:
		if (position % MAZE_SIZE == MAZE_SIZE - 1)
			break;
		walls[position + EAST] |= WEST_BIT;
		break;
	case SOUTH_BIT:
		if (position / MAZE_SIZE == 0)
			break;
		walls[position + SOUTH] |= NORTH_BIT;
		break;
	case WEST_BIT:
		if (position % MAZE_SIZE == 0)
			break;
		walls[position + WEST] |= EAST_BIT;
		break;
	case NORTH_BIT:
		if (position / MAZE_SIZE == MAZE_SIZE - 1)
			break;
		walls[position + NORTH] |= SOUTH_BIT;
		break;
	default:
		break;
	}
}

/**
 * @brief Place a new wall in the maze memory representation.
 *
 * If the wall was already there, it will do nothing.
 *
 * @return Whether the wall was built or not (i.e.: if it existed before).
 */
static bool place_wall(uint8_t bit)
{
	if (!wall_exists(current_position, bit)) {
		build_wall(current_position, bit);
		return true;
	}
	return false;
}

static void update_walls(void)
{
	bool windrose[4] = {false, false, false, false};

	switch (current_direction) {
	case EAST:
		windrose[0] = current_walls.front;
		windrose[1] = current_walls.right;
		windrose[2] = false;
		windrose[3] = current_walls.left;
		break;
	case SOUTH:
		windrose[0] = current_walls.left;
		windrose[1] = current_walls.front;
		windrose[2] = current_walls.right;
		windrose[3] = false;
		break;
	case WEST:
		windrose[0] = false;
		windrose[1] = current_walls.left;
		windrose[2] = current_walls.front;
		windrose[3] = current_walls.right;
		break;
	case NORTH:
		windrose[0] = current_walls.right;
		windrose[1] = false;
		windrose[2] = current_walls.left;
		windrose[3] = current_walls.front;
		break;
	default:
		break;
	}
	push_cell(current_position);
	if (windrose[0] && place_wall(EAST_BIT))
		push_cell(current_position + EAST);
	if (windrose[1] && place_wall(SOUTH_BIT))
		push_cell(current_position + SOUTH);
	if (windrose[2] && place_wall(WEST_BIT))
		push_cell(current_position + WEST);
	if (windrose[3] && place_wall(NORTH_BIT))
		push_cell(current_position + NORTH);
	walls[current_position] |= VISITED_BIT;
}

enum compass_direction search_direction(void)
{
	return current_direction;
}

uint8_t search_position(void)
{
	return current_position;
}

uint8_t search_distance(void)
{
	return distances[current_position];
}

static uint8_t left_distance(void)
{
	return distances[next_step_position(LEFT)];
}

static uint8_t front_distance(void)
{
	return distances[next_step_position(FRONT)];
}

static uint8_t right_distance(void)
{
	return distances[next_step_position(RIGHT)];
}

/**
 * @brief Initialize maze walls with borders and initial direction.
 *
 * Basically add walls to the maze perimeter and also create a side wall
 * in the initial location according to the initial direction of the mouse,
 * which can be "north" or "east".
 */
void initialize_maze_walls(void)
{
	int i;

	for (i = 0; i < MAZE_SIZE * MAZE_SIZE; i++)
		walls[i] = 0;

	for (i = 0; i < MAZE_SIZE; i++) {
		walls[MAZE_SIZE - 1 + i * MAZE_SIZE] |= EAST_BIT;
		walls[i] |= SOUTH_BIT;
		walls[i * MAZE_SIZE] |= WEST_BIT;
		walls[i + (MAZE_SIZE - 1) * MAZE_SIZE] |= NORTH_BIT;
	}

	walls[0] |= (initial_direction == NORTH) ? EAST_BIT : NORTH_BIT;
	walls[0] |= VISITED_BIT;
}

/**
 * @brief Initialize maze distances with respect to the center of the maze.
 *
 * The center is considered to be a 2x2 area in the exact middle of the maze.
 */
void initialize_distances_standard(void)
{
	int i;
	int j;

	for (i = 0; i < MAZE_SIZE; i++) {
		for (j = 0; j < MAZE_SIZE; j++) {
			distances[i + j * MAZE_SIZE] =
			    abs(i - MAZE_SIZE / 2) + abs(j - MAZE_SIZE / 2);
			if (i < MAZE_SIZE / 2)
				distances[i + j * MAZE_SIZE] -= 1;
			if (j < MAZE_SIZE / 2)
				distances[i + j * MAZE_SIZE] -= 1;
		}
	}
}

/**
 * @brief Initialize maze distances with respect to a given single coordinate.
 */
void initialize_distances_coordinate(int x, int y)
{
	int i;
	int j;

	for (i = 0; i < MAZE_SIZE; i++)
		for (j = 0; j < MAZE_SIZE; j++)
			distances[i + j * MAZE_SIZE] = abs(i - x) + abs(j - y);
}

/**
 * @brief Initialize maze distances with unique values (for testing).
 */
void initialize_distances_unique(void)
{
	int i;

	for (i = 0; i < MAZE_SIZE * MAZE_SIZE; i++)
		distances[i] = 255 - i;
}

enum step_direction best_neighbor_step(void)
{
	if (!current_walls.front && (front_distance() < search_distance()))
		return FRONT;
	if (!current_walls.left && (left_distance() < search_distance()))
		return LEFT;
	if (!current_walls.right && (right_distance() < search_distance()))
		return RIGHT;
	return BACK;
}

static uint8_t minimum_open_neighbor_distance(uint8_t cell)
{
	uint8_t minimum = 255;
	uint8_t neighbor;

	if (!wall_exists(cell, EAST_BIT)) {
		neighbor = distances[cell + EAST];
		minimum = (neighbor < minimum) ? neighbor : minimum;
	}
	if (!wall_exists(cell, SOUTH_BIT)) {
		neighbor = distances[cell + SOUTH];
		minimum = (neighbor < minimum) ? neighbor : minimum;
	}
	if (!wall_exists(cell, WEST_BIT)) {
		neighbor = distances[cell + WEST];
		minimum = (neighbor < minimum) ? neighbor : minimum;
	}
	if (!wall_exists(cell, NORTH_BIT)) {
		neighbor = distances[cell + NORTH];
		minimum = (neighbor < minimum) ? neighbor : minimum;
	}

	return minimum;
}

static void push_open_neighbors(uint8_t cell)
{
	if (!wall_exists(cell, EAST_BIT))
		push_cell(cell + EAST);
	if (!wall_exists(cell, SOUTH_BIT))
		push_cell(cell + SOUTH);
	if (!wall_exists(cell, WEST_BIT))
		push_cell(cell + WEST);
	if (!wall_exists(cell, NORTH_BIT))
		push_cell(cell + NORTH);
}

static void update_distances(void)
{
	uint8_t cell;
	uint8_t minimum;

	while (stack.size) {
		cell = pop_cell();
		minimum = minimum_open_neighbor_distance(cell);
		if (distances[cell] == 0)
			continue;
		if (distances[cell] == minimum + 1)
			continue;
		distances[cell] = minimum + 1;
		push_open_neighbors(cell);
	}
}

void move_search_position(enum step_direction step)
{
	enum compass_direction next;

	next = next_compass_direction(step);
	current_position += next;
	current_direction = next;
}

/**
 * @brief Initialize maze walls, distances and set current initial direction.
 */
void initialize_search(void)
{
	initialize_maze_walls();
	initialize_distances_standard();
	set_search_initial_state();
}

void search_update(bool left, bool front, bool right)
{
	set_current_walls(left, front, right);
	update_walls();
	update_distances();
}

enum step_direction search_step(bool left, bool front, bool right)
{
	enum step_direction step;

	search_update(left, front, right);
	step = best_neighbor_step();
	move_search_position(step);

	return step;
}
