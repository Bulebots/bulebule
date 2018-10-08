#include "solve.h"
#include "search.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <zmq.h>

static void *requester;

static void wait_response()
{
	char buffer[256] = { 0 };

	zmq_recv(requester, buffer, 256, 0);
}

static char encoded_direction(void)
{
	if (search_direction() == EAST)
		return 'E';
	if (search_direction() == SOUTH)
		return 'S';
	if (search_direction() == WEST)
		return 'W';
	if (search_direction() == NORTH)
		return 'N';
	return 'X';
}

void send_state()
{
	char state[2 * (MAZE_SIZE * MAZE_SIZE + 1) + 4];
	int x;

	state[0] = 'S';
	state[1] = search_position() % MAZE_SIZE;
	state[2] = search_position() / MAZE_SIZE;
	state[3] = encoded_direction();
	state[4] = 'C';
	for (x=0; x<MAZE_SIZE*MAZE_SIZE; x++) {
		state[x + 5] =
		    read_cell_distance_value(x);
	}
	state[261] = 'C';
	for (x=0; x<MAZE_SIZE*MAZE_SIZE; x++) {
		state[x + 262] =
		    read_cell_walls_value(x);
	}

	zmq_send(requester, state, 518, 0);
	wait_response();
}

struct walls_around read_walls(void)
{
	char walls[3] = { 0 };
	char position_state[4];
	struct walls_around walls_readings;

	position_state[0] = 'W';
	position_state[1] = search_position() % MAZE_SIZE;
	position_state[2] = search_position() / MAZE_SIZE;
	position_state[3] = encoded_direction();

	zmq_send(requester, position_state, 4, 0);
	zmq_recv(requester, walls, 3, 0);

	walls_readings.left = (bool)walls[0];
	walls_readings.front = (bool)walls[1];
	walls_readings.right = (bool)walls[2];
	return walls_readings;
}

int main(void)
{
	int rc;
	void *context = zmq_ctx_new();
	char buffer[256];

	requester = zmq_socket(context, ZMQ_REQ);
	rc = zmq_connect(requester, "tcp://127.0.0.1:6574");
	assert(rc == 0);
	zmq_send(requester, "reset", 5, 0);
	wait_response();

	set_goal_classic();
	set_target_goal();

	explore();

	set_target_goal();
	set_distances();
	send_state();

	return 0;
}
