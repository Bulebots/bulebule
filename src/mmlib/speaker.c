#include "speaker.h"

/**
 * Number of semitones above C, for each note in an octave.
 *
 * Notes are in order: {A, B, C, D, E, F, G}.
 */
uint8_t SEMITONES[8] = {9, 11, 0, 2, 4, 5, 7};

/**
 * Base or reference note, used for the pitch.
 */
uint8_t BASE_NOTE = 9;
uint8_t BASE_OCTAVE = 4;
float BASE_FREQUENCY = 440.;

/**
 * @brief Play a note through the speaker.
 *
 * Even if this function uses the scientific notation, notes are played with
 * the concert pitch (standard pitch). That means A above middle C is the
 * reference note, played at 440 Hz.
 *
 * @param[in] note Which note to play, in scientific notation.
 * @param[in] octave Which octave to play, in scientific notation.
 * @param[in] accidental Number of semitones to sum to the note.
 * @param[in] duration Duration of the note, in seconds.
 */
void speaker_play(char note, uint8_t octave, int8_t accidental, float duration)
{
	int16_t sound;
	float frequency;

	sound = SEMITONES[note - 'A'] - BASE_NOTE + (octave - BASE_OCTAVE) * 12;
	sound += accidental;
	frequency = pow(2, sound / 12.) * BASE_FREQUENCY;
	speaker_on(frequency);
	sleep_seconds(duration);
	speaker_off();
}
