#include <libintl.h>
#include <string.h>
#include "s_audio.h"
#include "s_resources.h"

#define _(String) gettext (String);

int sound_mix_buffers(Uint8 *dst, int dst_len, Uint8 *src, int src_len)
{
	int i;
	for (i=0; i < dst_len && i < src_len; i++) {
		int sample = (dst[i] - 127) + (src[i] - 127) + 127;
		if (sample < 0) sample = 0;
		if (sample > 255) sample = 255;
		dst[i] = sample;
	}
	return i;
}

void sound_loop(void *data, Uint8 *stream, int len)
{
	sound_t *snd = (sound_t*)data;
	int step, i;

	for (i=0;i<CHANNELS;i++) {
		if (snd->data[i]) {
			step = sound_mix_buffers(stream, len, snd->data[i] + snd->pos[i], snd->len[i] - snd->pos[i]);
			snd->pos[i] += step;
			if (snd->pos[i] == snd->len[i])	sound_stop(snd, i);
		}
	}
}

int sound_init(sound_t *snd)
{
	SDL_AudioSpec spec, req;
	memset(snd, 0, sizeof(sound_t));

	if (!is_sound) return ERR_OK;

    spec.freq = 11025;
    spec.format = AUDIO_U8;
    spec.channels = 1;
    spec.samples = 1024;
    spec.callback = sound_loop;
    spec.userdata = snd;
    if (SDL_OpenAudio(&spec, &req) < 0 ) {
    	return ERR_SDL;
    }

    if (spec.format != req.format || spec.freq != req.freq || spec.channels != req.channels) {
    	err_string = _("Cannot request proper audio format (U8, 11025, mono)");
    	return ERR_ENGINE;
    }

    SDL_PauseAudio(0);
    return ERR_OK;
}

void sound_clean()
{
	if (!is_sound) return;

	SDL_PauseAudio(1);
	SDL_CloseAudio();
}

void sound_play(sound_t *snd, int channel, wave_t *wav)
{
	if (!is_sound) return;

	snd->data[channel] = wav->data;
	snd->len[channel] = wav->len;
	snd->pos[channel] = 0;
}

void sound_stop(sound_t *snd, int channel)
{
	if (!is_sound) return;

	snd->data[channel] = NULL;
	snd->len[channel] = 0;
	snd->pos[channel] = 0;
}

int sound_isplay(sound_t *snd, int channel)
{
	if (!is_sound) return 1;

	if (snd->data[channel]) return 1;
	else return 0;
}

