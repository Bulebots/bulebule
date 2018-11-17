#include "path.h"

static bool straight = true;
static char *source;
static enum movement *destination;

/**
 * @brief Translate the source path when coming from a straight movement.
 *
 * @return Number of movements translated from source.
 */
static int from_straight(void)
{
	if (!strncmp(source, "F", 1)) {
		*destination++ = MOVE_FRONT;
		return 1;
	}
	if (!strncmp(source, "BF", 2)) {
		*destination++ = MOVE_START;
		return 1;
	}
	if (!strncmp(source, "LR", 2)) {
		*destination++ = MOVE_LEFT_TO_45;
	        straight = false;
		return 1;
	}
	if (!strncmp(source, "RL", 2)) {
		*destination++ = MOVE_RIGHT_TO_45;
	        straight = false;
		return 1;
	}
	if (!strncmp(source, "LF", 2)) {
		*destination++ = MOVE_LEFT_90;
		return 1;
	}
	if (!strncmp(source, "RF", 2)) {
		*destination++ = MOVE_RIGHT_90;
		return 1;
	}
	if (!strncmp(source, "LLR", 3)) {
		*destination++ = MOVE_LEFT_TO_135;
	        straight = false;
		return 2;
	}
	if (!strncmp(source, "RRL", 3)) {
		*destination++ = MOVE_RIGHT_TO_135;
	        straight = false;
		return 2;
	}
	if (!strncmp(source, "LLF", 3)) {
		*destination++ = MOVE_LEFT_180;
		return 2;
	}
	if (!strncmp(source, "RRF", 3)) {
		*destination++ = MOVE_RIGHT_180;
		return 2;
	}
	straight = false;
	return 0;
}

/**
 * @brief Translate the source path when coming from a diagonal movement.
 *
 * @return Number of movements translated from source.
 */
static int from_diagonal(void)
{
	if (!strncmp(source, "B", 1)) {
		*destination++ = MOVE_START;
		return 1;
	}
	if (!strncmp(source, "LF", 2)) {
		*destination++ = MOVE_LEFT_FROM_45;
		return 1;
	}
	if (!strncmp(source, "RF", 2)) {
		*destination++ = MOVE_RIGHT_FROM_45;
		return 1;
	}
	if (!strncmp(source, "LLR", 3)) {
		*destination++ = MOVE_LEFT_DIAGONAL;
		return 2;
	}
	if (!strncmp(source, "RRL", 3)) {
		*destination++ = MOVE_RIGHT_DIAGONAL;
		return 2;
	}
	if (!strncmp(source, "LLF", 3)) {
		*destination++ = MOVE_LEFT_FROM_135;
		return 2;
	}
	if (!strncmp(source, "RRF", 3)) {
		*destination++ = MOVE_RIGHT_FROM_135;
		return 2;
	}
	if (!strncmp(source, "LR", 2)) {
		*destination++ = MOVE_DIAGONAL;
		return 1;
	}
	if (!strncmp(source, "RL", 2)) {
		*destination++ = MOVE_DIAGONAL;
		return 1;
	}
	if (!strncmp(source, "L", 1)) {
		*destination++ = MOVE_LEFT;
		return 1;
	}
	if (!strncmp(source, "R", 1)) {
		*destination++ = MOVE_RIGHT;
		return 1;
	}
	return 0;
}

/**
 * @brief Make a smooth path out of a raw, exploration path.
 *
 * @param[in] raw_path Raw path to smooth.
 * @param[out] smooth_path Array to write the smooth path to.
 */
void make_smooth_path(char *raw_path, enum movement *smooth_path)
{
	source = raw_path;
	destination = smooth_path;
	while (true) {
		if (*source == '\0')
			break;
		if (*source == 'S') {
			*destination++ = MOVE_STOP;
			break;
		}
		if (*source == 'F')
			straight = true;
		if (straight)
			source += from_straight();
		if (!straight)
			source += from_diagonal();
	}
	*destination = MOVE_END;
}
