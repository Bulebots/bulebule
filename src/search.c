#include "search.h"

static uint8_t distances[MAZE_SIZE * MAZE_SIZE];
static uint8_t maze_walls[MAZE_SIZE * MAZE_SIZE];

static enum compass_direction initial_direction = NORTH;

static uint8_t current_position;
static enum compass_direction current_direction;

static struct data_queue {
	uint8_t buffer[MAZE_AREA];
	int head;
	int tail;
} queue;

struct cells_stack {
	int cells[MAX_TARGETS];
	uint8_t size;
};

static struct cells_stack goal_cells;
static struct cells_stack target_cells;

static void queue_push(uint8_t data)
{
	queue.buffer[queue.head++] = data;
}

static uint8_t queue_pop(void)
{
	return queue.buffer[queue.tail++];
}

uint8_t read_cell_distance_value(uint8_t cell)
{
	return distances[cell];
}

uint8_t read_cell_walls_value(uint8_t cell)
{
	return maze_walls[cell];
}

/**
 * @brief Add new goal coordinates.
 */
void add_goal(int x, int y)
{
	goal_cells.cells[goal_cells.size++] = x + y * MAZE_SIZE;
}

/**
 * @brief Set goal according to the classic micromouse competition rules.
 */
void set_goal_classic(void)
{
	add_goal(7, 7);
	add_goal(7, 8);
	add_goal(8, 7);
	add_goal(8, 8);
}

/**
 * @brief Add new target cell.
 */
static void add_target(uint8_t cell)
{
	target_cells.cells[target_cells.size++] = cell;
}

/**
 * @brief Set new target cell.
 */
void set_target_cell(uint8_t cell)
{
	target_cells.size = 0;
	add_target(cell);
}

/**
 * @brief Set the goal as target.
 */
void set_target_goal(void)
{
	int i;

	target_cells.size = 0;
	for (i = 0; i < goal_cells.size; i++)
		add_target(goal_cells.cells[i]);
}

void set_search_initial_direction(enum compass_direction direction)
{
	initial_direction = direction;
}

void set_search_initial_state(void)
{
	current_position = 0;
	current_direction = initial_direction;
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
 * @brief Check if there is a wall at the current position and provided side.
 *
 * @param[in] side Where to look for the wall.
 */
bool current_side_wall(enum step_direction side)
{
	uint8_t bit = 0;
	enum compass_direction direction;

	direction = next_compass_direction(side);
	switch (direction) {
	case EAST:
		bit = EAST_BIT;
		break;
	case SOUTH:
		bit = SOUTH_BIT;
		break;
	case WEST:
		bit = WEST_BIT;
		break;
	case NORTH:
		bit = NORTH_BIT;
		break;
	default:
		break;
	}
	return (maze_walls[current_position] & bit);
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
	return (maze_walls[position] & bit);
}

static void build_wall(uint8_t position, uint8_t bit)
{
	maze_walls[position] |= bit;
	switch (bit) {
	case EAST_BIT:
		if (position % MAZE_SIZE == MAZE_SIZE - 1)
			break;
		maze_walls[position + EAST] |= WEST_BIT;
		break;
	case SOUTH_BIT:
		if (position / MAZE_SIZE == 0)
			break;
		maze_walls[position + SOUTH] |= NORTH_BIT;
		break;
	case WEST_BIT:
		if (position % MAZE_SIZE == 0)
			break;
		maze_walls[position + WEST] |= EAST_BIT;
		break;
	case NORTH_BIT:
		if (position / MAZE_SIZE == MAZE_SIZE - 1)
			break;
		maze_walls[position + NORTH] |= SOUTH_BIT;
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

void update_walls(struct walls_around walls)
{
	bool windrose[4] = {false, false, false, false};

	switch (current_direction) {
	case EAST:
		windrose[0] = walls.front;
		windrose[1] = walls.right;
		windrose[2] = false;
		windrose[3] = walls.left;
		break;
	case SOUTH:
		windrose[0] = walls.left;
		windrose[1] = walls.front;
		windrose[2] = walls.right;
		windrose[3] = false;
		break;
	case WEST:
		windrose[0] = false;
		windrose[1] = walls.left;
		windrose[2] = walls.front;
		windrose[3] = walls.right;
		break;
	case NORTH:
		windrose[0] = walls.right;
		windrose[1] = false;
		windrose[2] = walls.left;
		windrose[3] = walls.front;
		break;
	default:
		break;
	}
	if (windrose[0])
		place_wall(EAST_BIT);
	if (windrose[1])
		place_wall(SOUTH_BIT);
	if (windrose[2])
		place_wall(WEST_BIT);
	if (windrose[3])
		place_wall(NORTH_BIT);
	maze_walls[current_position] |= VISITED_BIT;
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
 * @brief Initialize maze walls with borders.
 *
 * Basically add walls to the maze perimeter.
 */
void initialize_maze_walls(void)
{
	int i;

	for (i = 0; i < MAZE_SIZE * MAZE_SIZE; i++)
		maze_walls[i] = 0;

	for (i = 0; i < MAZE_SIZE; i++) {
		maze_walls[MAZE_SIZE - 1 + i * MAZE_SIZE] |= EAST_BIT;
		maze_walls[i] |= SOUTH_BIT;
		maze_walls[i * MAZE_SIZE] |= WEST_BIT;
		maze_walls[i + (MAZE_SIZE - 1) * MAZE_SIZE] |= NORTH_BIT;
	}
}

enum step_direction best_neighbor_step(struct walls_around walls)
{
	if (!walls.front && (front_distance() < search_distance()))
		return FRONT;
	if (!walls.left && (left_distance() < search_distance()))
		return LEFT;
	if (!walls.right && (right_distance() < search_distance()))
		return RIGHT;
	return BACK;
}

static void queue_push_breath(uint8_t cell, uint8_t distance)
{
	if (distances[cell] <= distance)
		return;
	distances[cell] = distance;
	queue_push(cell);
}

static void update_distances_breath(void)
{
	uint8_t cell;
	uint8_t distance;

	while (queue.head != queue.tail) {
		cell = queue_pop();
		distance = distances[cell] + 1;
		if (!wall_exists(cell, EAST_BIT))
			queue_push_breath(cell + EAST, distance);
		if (!wall_exists(cell, SOUTH_BIT))
			queue_push_breath(cell + SOUTH, distance);
		if (!wall_exists(cell, WEST_BIT))
			queue_push_breath(cell + WEST, distance);
		if (!wall_exists(cell, NORTH_BIT))
			queue_push_breath(cell + NORTH, distance);
	}
}

/**
 * @brief Reset maze distances and queue.
 *
 * To be executed as the first flood-fill step, before pushing the target
 * cells to the queue.
 */
static void _reset_distances_and_queue(void)
{
	int i;

	for (i = 0; i < MAZE_AREA; i++)
		distances[i] = MAX_DISTANCE;
	queue.head = 0;
	queue.tail = 0;
}

/**
 * @brief Set maze distances with respect to the target.
 */
void set_distances(void)
{
	int i;
	int cell;

	_reset_distances_and_queue();
	for (i = 0; i < target_cells.size; i++) {
		cell = target_cells.cells[i];
		distances[cell] = 0;
		queue_push(cell);
	}
	update_distances_breath();
}

void move_search_position(enum step_direction step)
{
	enum compass_direction next;

	next = next_compass_direction(step);
	current_position += next;
	current_direction = next;
}

/**
 * @brief Return whether the current cell has already been visited before.
 */
bool current_cell_is_visited(void)
{
	return (bool)(maze_walls[current_position] & VISITED_BIT);
}

/**
 * @brief Return the walls around at the current position.
 */
struct walls_around current_walls_around(void)
{
	struct walls_around walls;
	uint8_t cell;

	cell = maze_walls[current_position];
	switch (current_direction) {
	case EAST:
		walls.left = (bool)(cell & NORTH_BIT);
		walls.front = (bool)(cell & EAST_BIT);
		walls.right = (bool)(cell & SOUTH_BIT);
		break;
	case SOUTH:
		walls.left = (bool)(cell & EAST_BIT);
		walls.front = (bool)(cell & SOUTH_BIT);
		walls.right = (bool)(cell & WEST_BIT);
		break;
	case WEST:
		walls.left = (bool)(cell & SOUTH_BIT);
		walls.front = (bool)(cell & WEST_BIT);
		walls.right = (bool)(cell & NORTH_BIT);
		break;
	case NORTH:
		walls.left = (bool)(cell & WEST_BIT);
		walls.front = (bool)(cell & NORTH_BIT);
		walls.right = (bool)(cell & EAST_BIT);
		break;
	default:
		break;
	}

	return walls;
}

/**
 * @brief Find an unexplored and potentially interesting cell.
 */
uint8_t find_unexplored_interesting_cell(void)
{
	uint8_t interesting = 0;
	uint8_t backed_up_position;
	enum compass_direction backed_up_direction;
	enum step_direction step;

	/* Back up position and direction */
	backed_up_position = current_position;
	backed_up_direction = current_direction;

	set_search_initial_state();
	set_target_goal();
	set_distances();
	while (search_distance() > 0) {
		step = best_neighbor_step(current_walls_around());
		move_search_position(step);
		if (!current_cell_is_visited()) {
			interesting = current_position;
			break;
		}
	}

	/* Recover backed up position and direction */
	current_position = backed_up_position;
	current_direction = backed_up_direction;
	return interesting;
}
