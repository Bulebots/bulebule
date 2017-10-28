#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <zmq.h>

#define MAZE_SIZE 16
#define MAZE_AREA 16 * 16

#define VISITED_BIT 1
#define EAST_BIT 2
#define SOUTH_BIT 4
#define WEST_BIT 8
#define NORTH_BIT 16

enum compass_direction {
	EAST = 1,
	SOUTH = -MAZE_SIZE,
	WEST = -1,
	NORTH = MAZE_SIZE,
};
enum step_direction {
	NONE = -1,
	LEFT = 0,
	FRONT = 1,
	RIGHT = 2,
	BACK = 3
};

enum compass_direction initial_direction = EAST;

uint8_t distances[MAZE_SIZE * MAZE_SIZE];
uint8_t walls[MAZE_SIZE * MAZE_SIZE];

uint8_t current_position;
enum compass_direction current_direction;
struct sensors_detection {
	bool front : 1;
	bool left : 1;
	bool right : 1;
} current_walls;

void *requester;

void wait_response()
{
	char buffer[256] = { 0 };

	zmq_recv(requester, buffer, 256, 0);
	printf("%s\n", buffer);
}

void send_discovery()
{
	char discovery_state[2 * (MAZE_SIZE * MAZE_SIZE + 2)];
	int x;

	discovery_state[0] = 'D';
	for (x=0; x<MAZE_SIZE*MAZE_SIZE; x++) {
		discovery_state[x + 2] =
		    distances[x];
		discovery_state[x + 259] =
		    walls[x];
	}
	discovery_state[1] = 'C';
	discovery_state[258] = 'C';

	zmq_send(requester, discovery_state, 515, 0);
	wait_response();
}

void read_walls()
{
	char buffer[3] = { 0 };
	char position_state[4];

	position_state[0] = 'P';
	position_state[1] = current_position % MAZE_SIZE;
	position_state[2] = current_position / MAZE_SIZE;
	position_state[3] = current_direction;

	zmq_send(requester, position_state, 4, 0);
	zmq_recv(requester, buffer, 3, 0);

	current_walls.left = (bool)buffer[0];
	current_walls.front = (bool)buffer[1];
	current_walls.right = (bool)buffer[2];
}

enum compass_direction next_compass_direction(enum step_direction step)
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
	if (step == FRONT) {
		return current_direction;
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
	return -current_direction;
}

/**
 * @brief Return the position after a given step.
 */
uint8_t next_step_position(enum step_direction step)
{
	return current_position + next_compass_direction(step);
}

void create_wall_north(void)
{
	walls[current_position] |= NORTH_BIT;
	// TODO: create wall in adyacent cell as well
}

void create_wall_east(void)
{
	walls[current_position] |= EAST_BIT;
	// TODO: create wall in adyacent cell as well
}

void create_wall_south(void)
{
	walls[current_position] |= SOUTH_BIT;
	// TODO: create wall in adyacent cell as well
}

void create_wall_west(void)
{
	walls[current_position] |= WEST_BIT;
	// TODO: create wall in adyacent cell as well
}

void update_walls(void)
{
	bool windrose[4];

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
	if (windrose[0])
		create_wall_east();
	if (windrose[1])
		create_wall_south();
	if (windrose[2])
		create_wall_west();
	if (windrose[3])
		create_wall_north();
	walls[current_position] |= VISITED_BIT;
}

uint8_t current_distance()
{
	return distances[current_position];
}

uint8_t left_distance()
{
	return distances[next_step_position(LEFT)];
}

uint8_t front_distance()
{
	return distances[next_step_position(FRONT)];
}

uint8_t right_distance()
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
static void initialize_maze_walls(void)
{
	int i;

	for (i=0; i<MAZE_SIZE*MAZE_SIZE; i++)
		walls[i] = 0;

	for (i=0; i<MAZE_SIZE; i++) {
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
static void initialize_distances_standard(void)
{
	int i;
	int j;

	for (i=0; i<MAZE_SIZE; i++) {
		for (j=0; j<MAZE_SIZE; j++) {
			distances[i + j * MAZE_SIZE] = abs(i - MAZE_SIZE / 2) +
						       abs(j - MAZE_SIZE / 2);
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
static void initialize_distances_coordinate(int x, int y)
{
	int i;
	int j;

	for (i=0; i<MAZE_SIZE; i++)
		for (j=0; j<MAZE_SIZE; j++)
			distances[i + j * MAZE_SIZE] = abs(i - x) + abs(j - y);
}

/**
 * @brief Initialize maze distances with unique values (for testing).
 */
static void initialize_distances_unique()
{
	int i;

	for (i=0; i<MAZE_SIZE*MAZE_SIZE; i++)
		distances[i] = 255 - i;
}

enum step_direction best_neighbor_step(void)
{
	if (!current_walls.front && (front_distance() < current_distance()))
		return FRONT;
	if (!current_walls.left && (left_distance() < current_distance()))
		return LEFT;
	if (!current_walls.right && (right_distance() < current_distance()))
		return RIGHT;
	return BACK;
}

void move(enum step_direction step)
{
	enum compass_direction next;

	next = next_compass_direction(step);
	current_position += next;
	current_direction = next;
}

/**
 * @brief Initialize maze walls, distances and set current initial direction.
 */
void initialize(void)
{
	initialize_maze_walls();
	initialize_distances_standard();
	current_position = 0;
	current_direction = initial_direction;
	current_walls.front = 0;
	current_walls.left = 1;
	current_walls.right = 1;
}

int main (void)
{
	int i;
	int rc;
	void *context = zmq_ctx_new();
	char buffer[256];
	enum step_direction step;

	requester = zmq_socket(context, ZMQ_REQ);
	rc = zmq_connect(requester, "tcp://127.0.0.1:6574");
	assert(rc == 0);

	initialize();

	for (i=0; i<20; i++) {

		read_walls();
		update_walls();
		printf("Walls: %d, %d, %d\n", current_walls.left, current_walls.front, current_walls.right);
		// TODO: update_distances();
		send_discovery();

		step = best_neighbor_step();
		printf("Step: %d\n", step);

		read(STDIN_FILENO, buffer, sizeof(buffer));

		move(step);
		printf("Current position: %d\n", current_position);
	}

	return 0;
}
