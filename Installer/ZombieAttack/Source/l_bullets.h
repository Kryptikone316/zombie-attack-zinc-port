#ifndef BULLETS_H_
#define BULLETS_H_

#include <SDL.h>
#include "s_character.h"
#include "e_codes.h"

#define MAX_BULLETS         64
#define BULLETS_MAX_TIME    4

typedef struct {
	Sint16 posx, posy;
	direction_t direct;
} bullet_t;

typedef struct {
	SDL_Surface *left, *right;
	bullet_t bullet[MAX_BULLETS];
	Uint8 bullets_count;
	Sint16 curtime;
} bullets_buff_t;


int bullets_init(bullets_buff_t *bullets);
void bullets_clean(bullets_buff_t *bullets);
void bullets_add(bullets_buff_t *bullets, Uint16 posx, Uint16 posy, direction_t direct);
void bullets_remove(bullets_buff_t *bullets, int index);
void bullets_update(bullets_buff_t *bullets, Uint32 time);
void bullets_draw(bullets_buff_t *bullets, SDL_Surface *screen);


#endif /* BULLETS_H_ */
