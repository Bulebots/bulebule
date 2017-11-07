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
	position_state[1] = search_position() % MAZE_SIZE;
	position_state[2] = search_position() / MAZE_SIZE;
	if (search_direction() == EAST)
		position_state[3] = 'E';
	else if (search_direction() == SOUTH)
		position_state[3] = 'S';
	else if (search_direction() == WEST)
		position_state[3] = 'W';
	else if (search_direction() == NORTH)
		position_state[3] = 'N';
	else
		position_state[3] = 'X';

	zmq_send(requester, position_state, 4, 0);
	zmq_recv(requester, buffer, 3, 0);

	left_wall = (bool)buffer[0];
	front_wall = (bool)buffer[1];
	right_wall = (bool)buffer[2];
}

int main (void)
{
	int rc;
	void *context = zmq_ctx_new();
	char buffer[256];
	enum step_direction step;

	requester = zmq_socket(context, ZMQ_REQ);
	rc = zmq_connect(requester, "tcp://127.0.0.1:6574");
	assert(rc == 0);

	initialize_search();

	read(STDIN_FILENO, buffer, sizeof(buffer));
	while (search_distance() > 0) {
		read_walls();

		search_update(left_wall, front_wall, right_wall);

		send_discovery();

		step = best_neighbor_step();

		read(STDIN_FILENO, buffer, sizeof(buffer));
		// usleep(100000);

		move_search_position(step);
	}
	read_walls();
	search_update(left_wall, front_wall, right_wall);
	send_discovery();

	return 0;
}
