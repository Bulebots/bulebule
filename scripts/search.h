#ifndef __SEARCH_H
#define __SEARCH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

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

uint8_t distances[MAZE_SIZE * MAZE_SIZE];
uint8_t walls[MAZE_SIZE * MAZE_SIZE];

enum compass_direction search_direction(void);
void move_search_position(enum step_direction step);
enum step_direction best_neighbor_step(void);
uint8_t search_position(void);
uint8_t search_distance(void);
enum step_direction search_step(bool left, bool front, bool right);
void initialize_search(void);
void search_update(bool left, bool front, bool right);

#endif /* __SEARCH_H */
