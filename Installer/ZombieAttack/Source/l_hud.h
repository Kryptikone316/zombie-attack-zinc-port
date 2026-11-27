/*
 * l_hud.h
 *
 * Hud layer collect data from different game objects and draw this
 * information as a hud section
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 */

#ifndef L_HUD_H_
#define L_HUD_H_

#include <SDL.h>
#include "e_codes.h"
#include "s_font.h"
#include "l_enviroment.h"
#include "l_player.h"

typedef struct {

	SDL_Surface 	**glyphs_ptr;
	player_t 		*player;
	env_t			*env;

	SDL_Surface 	*info_day;
	SDL_Surface 	*gun_day;
	SDL_Surface 	*life_day;

	SDL_Surface 	*info_night;
	SDL_Surface 	*gun_night;
	SDL_Surface 	*life_night;

	SDL_Surface 	*warm_label;

	Uint16           last_nday;
	mtime_t          last_mtime;
	Sint16			 last_score;
	Uint16			 last_level;

} hud_t;

int hud_init(hud_t *hud, env_t *env, player_t *player, SDL_Surface **glyphs_ptr);
void hud_clean(hud_t *hud);

void hud_draw(hud_t *hud, SDL_Surface *screen);

#endif
