#ifndef FONT_H_
#define FONT_H_

#include <SDL.h>
#include "e_codes.h"

typedef struct {
	SDL_Surface *small[256];
	SDL_Surface *big[256];
} font_t;

int font_init(font_t *font);
void font_clean(font_t *font);
SDL_Surface * font_render_text(const char *text, SDL_Surface **font, Uint8 r, Uint8 g, Uint8 b);

#endif /* FONT_H_ */
