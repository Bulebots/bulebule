#include "search.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <zmq.h>

void *requester;

bool left_wall;
bool front_wall;
bool right_wall;

extern uint8_t walls[];
extern uint8_t distances[];

void wait_response()
{
	char buffer[256] = { 0 };

	zmq_recv(requester, buffer, 256, 0);
}

char encoded_direction(void)
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
	char state[2 * (MAZE_SIZE * MAZE_SIZE + 2) + 3];
	int x;

	state[0] = 'S';
	state[1] = search_position() % MAZE_SIZE;
	state[2] = search_position() / MAZE_SIZE;
	state[3] = encoded_direction();
	for (x=0; x<MAZE_SIZE*MAZE_SIZE; x++) {
		state[x + 5] =
		    distances[x];
		state[x + 262] =
		    walls[x];
	}
	state[4] = 'C';
	state[261] = 'C';

	zmq_send(requester, state, 518, 0);
	wait_response();
}

void read_walls()
{
	char walls[3] = { 0 };
	char position_state[4];

	position_state[0] = 'W';
	position_state[1] = search_position() % MAZE_SIZE;
	position_state[2] = search_position() / MAZE_SIZE;
	position_state[3] = encoded_direction();

	zmq_send(requester, position_state, 4, 0);
	zmq_recv(requester, walls, 3, 0);

	left_wall = (bool)walls[0];
	front_wall = (bool)walls[1];
	right_wall = (bool)walls[2];
}

int main(void)
{
	int rc;
	void *context = zmq_ctx_new();
	char buffer[256];
	enum step_direction step;

	requester = zmq_socket(context, ZMQ_REQ);
	rc = zmq_connect(requester, "tcp://127.0.0.1:6574");
	assert(rc == 0);
	zmq_send(requester, "reset", 5, 0);
	wait_response();

	initialize_search();

	while (search_distance() > 0) {
		read_walls();

		search_update(left_wall, front_wall, right_wall);

		send_state();

		step = best_neighbor_step();

		move_search_position(step);
	}
	read_walls();
	search_update(left_wall, front_wall, right_wall);
	send_state();

	return 0;
}
