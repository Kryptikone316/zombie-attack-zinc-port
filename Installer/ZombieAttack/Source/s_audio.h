#ifndef SOUND_H_
#define SOUND_H_

#include <SDL.h>
#include "e_codes.h"

/*
 * Wavetable is portion of sound ready to pass to sound playing object.
 * Can be loaded from resources.c
 */

typedef struct {
	Uint8     *data;
	Uint32     len;
} wave_t;

/*
 * We have 3 channels, audio loop mix this channels together using pos and len values
 * to detect position and end of the stream for each channel. After pos == len channel
 * was disabled throught setting data, len and pos to 0
 */

#define CHN_SURROUND 0
#define CHN_ZOMBIE   1
#define CHN_HIT      2
#define CHN_PLAYER   3
#define CHANNELS     4

typedef struct {
	Uint8         *data[CHANNELS];
	Uint32         len[CHANNELS];
	Uint32         pos[CHANNELS];
} sound_t;

int sound_init(sound_t *snd);
void sound_clean();
void sound_play(sound_t *snd, int channel, wave_t *wav);
void sound_stop(sound_t *snd, int channel);
int sound_isplay(sound_t *snd, int channel);

#endif /* SOUND_H_ */
