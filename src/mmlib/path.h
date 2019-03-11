#ifndef __PATH_H
#define __PATH_H

#include <stdbool.h>
#include <string.h>

enum path_language {
	PATH_SAFE,      /**< Do not generate smooth diagonals */
	PATH_DIAGONALS, /**< Generate smooth diagonals */
	LANGUAGES_COUNT,
};

enum movement {
	MOVE_END,
	MOVE_START,
	MOVE_STOP,
	MOVE_FRONT,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_BACK,
	MOVE_LEFT_90,
	MOVE_RIGHT_90,
	MOVE_LEFT_180,
	MOVE_RIGHT_180,
	MOVE_LEFT_TO_45,
	MOVE_RIGHT_TO_45,
	MOVE_LEFT_TO_135,
	MOVE_RIGHT_TO_135,
	MOVE_LEFT_FROM_45,
	MOVE_RIGHT_FROM_45,
	MOVE_LEFT_FROM_135,
	MOVE_RIGHT_FROM_135,
	MOVE_DIAGONAL,
	MOVE_LEFT_DIAGONAL,
	MOVE_RIGHT_DIAGONAL,
	MOVE_NONE,
};

void make_smooth_path(char *raw_path, enum movement *smooth_path,
		      enum path_language language);

#endif /* __PATH_H */
