#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <zmq.h>

#define MAZE_SIZE 16

#define VISITED_BIT 1
#define EAST_BIT 2
#define SOUTH_BIT 4
#define WEST_BIT 8
#define NORTH_BIT 16

enum direction { EAST = 0, SOUTH = 1, WEST = 2, NORTH = 3 };

enum direction initial_direction = EAST;

uint8_t distances[MAZE_SIZE][MAZE_SIZE];
uint8_t walls[MAZE_SIZE][MAZE_SIZE];

struct {
	uint8_t x;
	uint8_t y;
	uint8_t direction;
} position;

void wait_response(void *req)
{
	char buffer[256] = { 0 };

	zmq_recv(req, buffer, 256, 0);
	printf("%s\n", buffer);
}

void send_discovery(void *req)
{
	char discovery_state[2 * (MAZE_SIZE * MAZE_SIZE + 2)];
	int x;
	int y;

	discovery_state[0] = 'D';
	for (x=0; x<MAZE_SIZE; x++) {
		for (y=0; y<MAZE_SIZE; y++) {
			discovery_state[x + y * MAZE_SIZE + 2] =
			    distances[x][y];
			discovery_state[x + y * MAZE_SIZE + 259] =
			    walls[x][y];
		}
	}
	discovery_state[1] = 'C';
	discovery_state[258] = 'C';

	zmq_send(req, discovery_state, 515, 0);
	wait_response(req);
}

void send_position(void *req)
{
	char position_state[4];
	position_state[0] = 'P';
	position_state[1] = position.x;
	position_state[2] = position.y;
	position_state[3] = position.direction;
	zmq_send(req, position_state, 4, 0);
	wait_response(req);
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
	int j;

	for (i=0; i<MAZE_SIZE; i++)
		for (j=0; j<MAZE_SIZE; j++)
			walls[i][j] = 0;

	for (i=0; i<MAZE_SIZE; i++) {
		walls[MAZE_SIZE - 1][i] |= EAST_BIT;
		walls[i][0] |= SOUTH_BIT;
		walls[0][i] |= WEST_BIT;
		walls[i][MAZE_SIZE - 1] |= NORTH_BIT;
	}

	walls[0][0] |= (initial_direction == NORTH) ? EAST_BIT : NORTH_BIT;
	walls[0][0] |= VISITED_BIT;
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
			distances[i][j] = abs(i - MAZE_SIZE / 2) +
					  abs(j - MAZE_SIZE / 2);
			if (i < MAZE_SIZE / 2)
				distances[i][j] -= 1;
			if (j < MAZE_SIZE / 2)
				distances[i][j] -= 1;
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
			distances[i][j] = abs(i - x) + abs(j - y);
}

/**
 * @brief Initialize maze walls, distances and set current initial direction.
 */
void initialize(void)
{
	initialize_maze_walls();
	initialize_distances_standard();
	position.x = 0;
	position.y = 0;
	position.direction = initial_direction;
}

int main (void)
{
	int rc;
	void *context = zmq_ctx_new();
	void *req = zmq_socket(context, ZMQ_REQ);

	rc = zmq_connect(req, "tcp://127.0.0.1:6574");
	assert(rc == 0);

	initialize();

	send_position(req);
	send_discovery(req);

	return 0;
}
