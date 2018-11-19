#include "path.h"

static char *source;
static enum movement *destination;

struct translation {
	char *from;
	enum movement to;
};

#define FROM_STRAIGHT_SIZE 8
#define FROM_DIAGONAL_SIZE 10

static struct translation from_straight[FROM_STRAIGHT_SIZE] = {
    {"LR", MOVE_LEFT_TO_45},   {"RL", MOVE_RIGHT_TO_45},
    {"LF", MOVE_LEFT_90},      {"RF", MOVE_RIGHT_90},
    {"LLR", MOVE_LEFT_TO_135}, {"RRL", MOVE_RIGHT_TO_135},
    {"LLF", MOVE_LEFT_180},    {"RRF", MOVE_RIGHT_180},
};

static struct translation from_diagonal[FROM_DIAGONAL_SIZE] = {
    {"LR", MOVE_DIAGONAL},       {"RL", MOVE_DIAGONAL},
    {"LLR", MOVE_LEFT_DIAGONAL}, {"RRL", MOVE_RIGHT_DIAGONAL},
    {"LF", MOVE_LEFT_FROM_45},   {"RF", MOVE_RIGHT_FROM_45},
    {"LLF", MOVE_LEFT_FROM_135}, {"RRF", MOVE_RIGHT_FROM_135},
};

/**
 * @brief Translate the source path to a smooth path.
 *
 * @param[in] dictionary The collection of possible translations.
 * @param[in] words Number of translations in the dictionary.
 */
static void translate(struct translation *dictionary, int words)
{
	struct translation candidate;
	char *pattern;
	int pattern_length;

	for (int i = 0; i < words; i++) {
		candidate = dictionary[i];
		pattern = candidate.from;
		pattern_length = strlen(pattern);
		if (!strncmp(source, pattern, pattern_length)) {
			*destination++ = candidate.to;
			source += pattern_length - 1;
			return;
		}
	}
}

/**
 * @brief Make a smooth path out of a raw, exploration path.
 *
 * @param[in] raw_path Raw path to smooth.
 * @param[out] smooth_path Array to write the smooth path to.
 */
void make_smooth_path(char *raw_path, enum movement *smooth_path)
{
	bool straight = false;

	source = raw_path;
	destination = smooth_path;
	while (true) {
		if (*source == '\0')
			break;
		if (*source == 'B') {
			*destination++ = MOVE_START;
			source++;
			continue;
		}
		if (*source == 'S') {
			*destination++ = MOVE_STOP;
			break;
		}
		if (*source == 'F') {
			straight = true;
			*destination++ = MOVE_FRONT;
			source++;
			continue;
		}
		if (straight)
			translate(from_straight, FROM_STRAIGHT_SIZE);
		else
			translate(from_diagonal, FROM_DIAGONAL_SIZE);
		straight = false;
	}
	*destination = MOVE_END;
}
