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
 * @brief Set the frequency for the speaker.
 *
 * Frequency is set modulating the PWM signal sent to the speaker.
 *
 * @param[in] hz Frequency, in Hertz.
 */
static void speaker_set_frequency(float hz)
{
	uint16_t period;

	period = (uint16_t)(SPEAKER_BASE_FREQUENCY_HZ / hz);
	timer_set_period(TIM1, period);
	timer_set_oc_value(TIM1, TIM_OC3, period / 2);
}

/**
 * @brief Turn on the speaker to play the set frequency.
 */
static void speaker_on(void)
{
	timer_enable_counter(TIM1);
	timer_enable_oc_output(TIM1, TIM_OC3);
}

/**
 * @brief Turn off the speaker.
 */
static void speaker_off(void)
{
	timer_disable_counter(TIM1);
	timer_disable_oc_output(TIM1, TIM_OC3);
}

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
 *
 * @note The speaker and emitters both share TIM1. While playing a note, the
 * emitters will be completely disabled and enabled back only after finishing
 * playing the note.
 */
void speaker_play(char note, uint8_t octave, int8_t accidental, float duration)
{
	int16_t sound;
	float frequency;

	sound = SEMITONES[note - 'A'] - BASE_NOTE + (octave - BASE_OCTAVE) * 12;
	sound += accidental;
	frequency = pow(2, sound / 12.) * BASE_FREQUENCY;
	setup_speaker();
	speaker_set_frequency(frequency);
	speaker_on();
	sleep_ticks((uint32_t)(duration * 1000));
	speaker_off();
	setup_emitters();
}
