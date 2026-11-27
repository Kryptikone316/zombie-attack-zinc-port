/*
 * m_advance.h
 * This is advance menu, show each time when
 * player advance to next level
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

#ifndef M_ADVANCE_H_
#define M_ADVANCE_H_

#include <SDL.h>
#include "s_font.h"
#include "l_player.h"


typedef struct {
	Uint32 code;
	int active;
	char *title;
	SDL_Surface 	*title_sel;
	SDL_Surface 	*title_nosel;
	SDL_Surface 	*title_dis;
} adv_option_t;

typedef struct {
	font_t 			*font;

	SDL_Rect 		outer_box;
	SDL_Rect 		inner_box;

	SDL_Surface 	*title;

	int				curopt;
	adv_option_t 	options[10];
} advance_t;


int advance_init(advance_t * advance, font_t *font);
void advance_clean(advance_t * advance);

Uint32 advance_choose(advance_t * advance, player_t *player, SDL_Surface *screen);
void advance_draw_menu(advance_t * advance, SDL_Surface *screen);

#endif /* M_ADVANCE_H_ */
