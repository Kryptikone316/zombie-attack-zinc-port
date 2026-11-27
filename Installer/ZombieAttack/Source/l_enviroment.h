/*
 * l_enviroment.h
 * This is used for draw background, enviroment like day time,
 * background objects (sun, chicken) ...
 *
 * Copyright (C) 2009 Kamil Pawlowski <kamilpe@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef L_ENVIROMENT_H_
#define L_ENVIROMENT_H_

#include <SDL.h>

/* time of the day in miliseconds */
// FIXME: Should be 3 * 60 * 100
#define DAYTIME 1 * 60 * 1000

typedef enum {
	TIME_AM,
	TIME_PM
} mtime_t;

typedef struct {
	SDL_Surface		*sky;
	SDL_Surface		*farm;
	SDL_Surface		*sunmoon;

	SDL_Rect		sky_rect;
	SDL_Rect		sunmoon_rect;
	SDL_Rect		sunmoon_pos;

	Uint16			nday;
	Uint32 			time;
	mtime_t 		mtime;

} env_t;

int env_init(env_t *env);
void env_clean(env_t *env);
void env_update(env_t *env, Uint32 time);
void env_draw_bg(env_t *env, SDL_Surface *screen);

#endif /* L_ENVIROMENT_H_ */
