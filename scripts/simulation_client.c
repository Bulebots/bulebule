#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <zmq.h>

#define MAZE_SIZE 16

#define VISITED_BIT 1
#define EAST_BIT 2
#define SOUTH_BIT 4
#define WEST_BIT 8
#define NORTH_BIT 16

enum direction { EAST, SOUTH, WEST, NORTH };

enum direction initial_direction = WEST;

uint8_t distances[MAZE_SIZE][MAZE_SIZE];
uint8_t walls[MAZE_SIZE][MAZE_SIZE];

char discovery_state[2 * (MAZE_SIZE * MAZE_SIZE + 1)];

void update_discovery_state(void)
{
	int x;
	int y;

	for (x=0; x<MAZE_SIZE; x++) {
		for (y=0; y<MAZE_SIZE; y++) {
			discovery_state[x + y * MAZE_SIZE + 1] =
			    distances[x][y];
			discovery_state[x + y * MAZE_SIZE + 258] =
			    walls[x][y];
		}
	}
	discovery_state[0] = 'C';
	discovery_state[257] = 'C';
}

/**
 * @brief Initialize maze walls with borders and initial direction.
 *
 * Basically add walls to the maze perimeter and also create a side wall
 * in the initial location according to the initial direction of the mouse,
 * which can be "north" or "west".
 */
void initialize_maze_walls(void)
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

int main (void)
{
	int rc;
	char buffer[10];
	void *context = zmq_ctx_new();
	void *req = zmq_socket(context, ZMQ_REQ);

	rc = zmq_connect(req, "tcp://127.0.0.1:6574");
	assert(rc == 0);

	initialize_maze_walls();

	update_discovery_state();
	zmq_send(req, discovery_state, 514, 0);

	zmq_recv(req, buffer, 10, 0);
	printf("%s\n", buffer);

	return 0;
}
