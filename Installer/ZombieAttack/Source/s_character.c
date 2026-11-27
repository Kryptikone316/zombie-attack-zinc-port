#include "s_character.h"

void
split_to_animation(SDL_Surface *from, SDL_Surface **film, direction_t direction,
		  int width, int height, int offset_x, int offset_y, int steps)
{
	int i, x, y;
	SDL_Rect src;
	SDL_Surface *frame, *rframe;
	char *rpixels, *pixels;
	direction_t rdirection;

	src.y = offset_y;
	src.h = height;
	src.w = width;

	if (direction == DR_LEFT) rdirection = DR_RIGHT;
	else rdirection = DR_LEFT;

	for (i=0;i<steps;i++) {
		src.x = offset_x + i*width;

		/* copy original frame */

		film[CALC_FRAME(direction, i)] = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 8, 0,0,0,0);
		frame = film[CALC_FRAME(direction, i)];

		SDL_SetPalette(frame, SDL_LOGPAL|SDL_PHYSPAL, from->format->palette->colors, 0, from->format->palette->ncolors);
		SDL_BlitSurface(from, &src, frame, NULL);
		SDL_SetColorKey(frame, SDL_SRCCOLORKEY, SDL_MapRGB(frame->format, 255,0,255));

		/* copy frame into reverse direction */

		film[CALC_FRAME(rdirection, i)] = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 8, 0,0,0,0);
		rframe = film[CALC_FRAME(rdirection, i)];

		SDL_SetPalette(rframe, SDL_LOGPAL|SDL_PHYSPAL, from->format->palette->colors, 0, from->format->palette->ncolors);
		SDL_FillRect(rframe, NULL, SDL_MapRGB(frame->format, 255,0,255));
		SDL_SetColorKey(rframe, SDL_SRCCOLORKEY, SDL_MapRGB(frame->format, 255,0,255));

		pixels = frame->pixels;
		rpixels = rframe->pixels;
		for (x=0; x<width; x++)
			for (y=0; y<height; y++)
				rpixels[y * rframe->pitch + x] = pixels[y * frame->pitch + (width - 1 - x)];
	}
}

void character_init_defaults(character_t *c)
{
	memset(c, 0, sizeof(character_t));
	c->state = ST_STOP;
	c->direct_look = DR_RIGHT;
	c->posx = 400;
	c->posy = 400;
	c->frametime_max = 1000;
	c->movetime_x = 100;
	c->movetime_y = 100;
}

void character_draw(character_t *c, SDL_Surface *screen)
{
	SDL_Rect dst;
	SDL_Surface **curmove = NULL;
	SDL_Surface *frame;

	switch (c->state) {
		case ST_STOP: curmove = c->stop; break;
		case ST_WALK: curmove = c->walk; break;
		case ST_SHOT: curmove = c->shot; break;
		case ST_DIE:
		case ST_DYING: curmove = c->die; break;
	}

	if (curmove[CALC_FRAME(c->direct_look, c->cur_frame)] == NULL) c->cur_frame = 0;
	frame = curmove[CALC_FRAME(c->direct_look, c->cur_frame)];

	dst.w = frame->w;
	dst.h = frame->h;
	dst.x = c->posx - frame->w / 2;
	dst.y = c->posy - frame->h;

	SDL_BlitSurface(frame, NULL, screen, &dst);
}

void character_update(character_t *c, Uint32 time)
{
	SDL_Surface **curmove = NULL;

	if (c->state == ST_DIE) {
		return;
	}

	/* aktualizuje pozycje obiektu */

	if (c->state == ST_WALK) {
		c->curtime_x += time;
		c->curtime_y += time;

		while (c->curtime_x > c->movetime_x) {
			c->curtime_x -= c->movetime_x;
			if (c->direct_move & DR_LEFT) c->posx -= 1;
			if (c->direct_move & DR_RIGHT) c->posx += 1;
		}

		while (c->curtime_y > c->movetime_y) {
			c->curtime_y -= c->movetime_y;
			if (c->direct_move & DR_UP) c->posy -= 1;
			if (c->direct_move & DR_DOWN) c->posy += 1;
		}
	}

	/* aktualizuje klatke animacji obiektu */

	switch (c->state) {
		case ST_STOP: curmove = c->stop; break;
		case ST_WALK: curmove = c->walk; break;
		case ST_SHOT: curmove = c->shot; break;
		case ST_DYING: curmove = c->die; break;
		case ST_DIE: return ;
	}

	/* przesuwam klatke animacji */

	c->frametime_cur += time;
	while (c->frametime_cur > c->frametime_max) {
		c->frametime_cur -= c->frametime_max;
		c->cur_frame++;
		/*
		 * w przypadku dojscia do konca animacji zaczynam od poczatku lub w przypadku
		 * umierania zostawiam ostatnia sekwencje i ustawiam odpowiedni status
		*/
		if (curmove[CALC_FRAME(c->direct_look, c->cur_frame)] == NULL) {
			if (c->state == ST_DYING) {
				c->state = ST_DIE;
				c->cur_frame --;
				return;
			} else {
				c->cur_frame = 0;
			}
		}
	}
}

void character_clean(character_t *c)
{
	int i;
	for (i=0; i < MAX_FRAMES*2; i++) {
		if (c->stop[i]) SDL_FreeSurface(c->stop[i]);
		if (c->walk[i]) SDL_FreeSurface(c->walk[i]);
		if (c->shot[i]) SDL_FreeSurface(c->shot[i]);
		if (c->die[i]) SDL_FreeSurface(c->die[i]);
	}
}
