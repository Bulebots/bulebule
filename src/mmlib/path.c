#include "path.h"

enum path_state {
	ORTHOGONAL, /**< When coming from an orthogonal movement */
	DIAGONAL,   /**< When coming from diagonal movement */
	PATH_STATES_COUNT,
};

struct translation {
	char *from;
	enum movement to;
};

// clang-format off
/**
 * @brief This dictionary defines different ways to translate to a smooth path.
 */
static struct translation *dictionary[LANGUAGES_COUNT][PATH_STATES_COUNT] = {
    [PATH_SAFE] = {
	[ORTHOGONAL] = (struct translation[]){
	    {"LF", MOVE_LEFT_90},   {"RF", MOVE_RIGHT_90},
	    {"LLF", MOVE_LEFT_180}, {"RRF", MOVE_RIGHT_180},
	    {"", MOVE_NONE},
	},
	[DIAGONAL] = (struct translation[]){
	    {"LL", MOVE_LEFT}, {"RR", MOVE_RIGHT},
	    {"LR", MOVE_LEFT}, {"RL", MOVE_RIGHT},
	    {"LF", MOVE_LEFT}, {"RF", MOVE_RIGHT},
	    {"", MOVE_NONE},
	},
    },
    [PATH_DIAGONALS] = {
	[ORTHOGONAL] = (struct translation[]){
	    {"LF", MOVE_LEFT_90},      {"RF", MOVE_RIGHT_90},
	    {"LR", MOVE_LEFT_TO_45},   {"RL", MOVE_RIGHT_TO_45},
	    {"LLF", MOVE_LEFT_180},    {"RRF", MOVE_RIGHT_180},
	    {"LLR", MOVE_LEFT_TO_135}, {"RRL", MOVE_RIGHT_TO_135},
	    {"", MOVE_NONE},
	},
	[DIAGONAL] = (struct translation[]){
	    {"LR", MOVE_DIAGONAL},       {"RL", MOVE_DIAGONAL},
	    {"LF", MOVE_LEFT_FROM_45},   {"RF", MOVE_RIGHT_FROM_45},
	    {"LLR", MOVE_LEFT_DIAGONAL}, {"RRL", MOVE_RIGHT_DIAGONAL},
	    {"LLF", MOVE_LEFT_FROM_135}, {"RRF", MOVE_RIGHT_FROM_135},
	    {"", MOVE_NONE},
	},
    },
};
// clang-format on

/**
 * @brief Translate the source path to a smooth path.
 *
 * @param[in] source Raw path to translate from.
 * @param[in] path_language Language to use for the translation.
 * @param[in] path_state The current path state.
 *
 * @return The translated next movement.
 */
static struct translation translate(char *source, enum path_language language,
				    enum path_state state)
{
	int length;
	struct translation *candidate;

	candidate = dictionary[language][state];
	while (true) {
		length = strlen(candidate->from);
		if (!length)
			break;
		if (!strncmp(source, candidate->from, length))
			return *candidate;
		candidate++;
	}
	return *candidate;
}

/**
 * @brief Make a smooth path out of a raw, exploration path.
 *
 * @param[in] source Raw path to smooth.
 * @param[out] destination Array to write the smooth path to.
 * @param[in] path_language Language to use for the translation.
 */
void make_smooth_path(char *source, enum movement *destination,
		      enum path_language language)
{
	enum path_state state = DIAGONAL;
	struct translation translated;

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
			source++;
			continue;
		}
		if (*source == 'F') {
			state = ORTHOGONAL;
			*destination++ = MOVE_FRONT;
			source++;
			continue;
		}
		translated = translate(source, language, state);
		if (translated.to != MOVE_NONE) {
			*destination++ = translated.to;
			source += strlen(translated.from) - 1;
		}
		state = DIAGONAL;
	}
	*destination = MOVE_END;
}
