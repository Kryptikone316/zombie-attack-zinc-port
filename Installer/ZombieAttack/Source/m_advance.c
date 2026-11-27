/*
 * m_advance.c
 * See header file for more information about module
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
#include <libintl.h>
#include "m_advance.h"

#define _(String) gettext (String)

int advance_init(advance_t * advance, font_t *font)
{
	int i;

	advance->font = font;

	advance->outer_box.x = 35;
	advance->outer_box.y = 102;
	advance->outer_box.w = 570;
	advance->outer_box.h = 275;

	advance->inner_box.x = 40;
	advance->inner_box.y = 107;
	advance->inner_box.w = 560;
	advance->inner_box.h = 265;

	advance->title = font_render_text(_("You have advance to next level, choose one upgrade:"), font->small, 0,240,255);
	advance->curopt = 0;
	advance->options[0].code = UPG_SPEED_MOVE;
	advance->options[0].title = _("Fast walking (Shift key)");
	advance->options[1].code = UPG_MORE_SHOOTS;
	advance->options[1].title = _("More shoots before warm");
	advance->options[2].code = UPG_FAST_SHOOTS;
	advance->options[2].title = _("Fast shooting");
	advance->options[3].code = UPG_LASER_AIM;
	advance->options[3].title = _("Laser aim");
	advance->options[4].code = UPG_DIR_SHOOTS;
	advance->options[4].title = _("Shoots in more directions");
	advance->options[5].code = UPG_TIME_STOP;
	advance->options[5].title = _("Freeze time for 5 seconds");
	advance->options[6].code = UPG_MORE_LIFE;
	advance->options[6].title = _("More life");
	advance->options[7].code = UPG_FAST_LIFE_RESTORE;
	advance->options[7].title = _("Faster live restore");
	advance->options[8].code = UPG_MORE_DAMAGE;
	advance->options[8].title = _("Bullets give more damages");
	advance->options[9].code = UPG_GRANADE;
	advance->options[9].title = _("Throw Granades (Space key)");

	/* render text surfaces based on title*/
	for (i=0;i<10;i++) {
		advance->options[i].title_nosel = font_render_text(advance->options[i].title, font->small, 0,240,255);
		advance->options[i].title_sel = font_render_text(advance->options[i].title, font->small, 255, 204, 6);
		advance->options[i].title_dis = font_render_text(advance->options[i].title, font->small, 157,158,156);
	}

	return ERR_OK;
}

void advance_clean(advance_t * advance)
{
	int i;
	for (i=0;i<10;i++) {
		SDL_FreeSurface(advance->options[i].title_dis);
		SDL_FreeSurface(advance->options[i].title_sel);
		SDL_FreeSurface(advance->options[i].title_nosel);
	}
	SDL_FreeSurface(advance->title);
}

Uint32 advance_choose(advance_t * advance, player_t *player, SDL_Surface *screen)
{
	SDL_Event event;
	int i;

	/* update options with player upg flags */
	for (i=0;i<10;i++) {
		if (advance->options[i].code & player->upg_flags) {
			advance->options[i].active = 0;
		} else {
			advance->options[i].active = 1;
		}
	}

	while (1) {

		while(SDL_WaitEvent(&event)) {

			advance_draw_menu(advance, screen);
			SDL_Flip(screen);

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_DOWN:
					advance->curopt++;
					break;
				case SDLK_UP:
					advance->curopt--;
					break;
				case SDLK_LEFT:
					if (advance->curopt >= 5) advance->curopt-=5;
					break;
				case SDLK_RIGHT:
					if (advance->curopt < 5) advance->curopt+=5;
					break;
				case SDLK_RETURN:
					if (advance->options[advance->curopt].active)
						return advance->options[advance->curopt].code;
					break;
				default:
					break;
				}
				if (advance->curopt < 0) advance->curopt = 0;
				if (advance->curopt > 9) advance->curopt = 9;
			}
		}
	}

	return ERR_ENGINE;
}


void advance_draw_menu(advance_t * advance, SDL_Surface *screen)
{
	SDL_Rect dst;
	int i;

	SDL_FillRect(screen, &advance->outer_box, SDL_MapRGB(screen->format, 87, 35, 0));
	SDL_FillRect(screen, &advance->inner_box, SDL_MapRGB(screen->format, 103, 63, 3));

	dst.x = (600 - advance->title->w) / 2 + advance->inner_box.x;
	dst.y = advance->inner_box.y + 2;
	dst.w = advance->title->w;
	dst.h = advance->title->h;

	SDL_BlitSurface(advance->title, NULL, screen, &dst);

	for (i=0;i<10;i++) {
		SDL_Rect rect_in, rect_out;
		int offx, offy;
		SDL_Surface *title;

		if (advance->options[i].active) {
			if (advance->curopt == i) {
				title = advance->options[i].title_sel;
			} else {
				title = advance->options[i].title_nosel;
			}
		} else {
			title = advance->options[i].title_dis;
		}

		/* dimension of option button */

		rect_in.w = 250;
		rect_in.h = 30;

		/* offset based on dimension of option button */

		if (i<5) {
			offx = 0;
			offy = (rect_in.h + 15) * i;
		} else {
			offx = rect_in.w + 15;
			offy = (rect_in.h + 15) * (i-5);
		}

		/* position including offset and inner_box area */

		rect_in.x = advance->inner_box.x + 18 + offx;
		rect_in.y = advance->inner_box.y + 35 + offy;

		/* if this is necessary draw selection box */
		if (advance->curopt == i) {
			rect_out.x = rect_in.x - 4;
			rect_out.y = rect_in.y - 4;
			rect_out.w = rect_in.w + 8;
			rect_out.h = rect_in.h + 8;
			if (advance->options[i].active) {
				SDL_FillRect(screen, &rect_out, SDL_MapRGB(screen->format, 255, 204, 6));
			} else {
				SDL_FillRect(screen, &rect_out, SDL_MapRGB(screen->format, 255, 0, 0));
			}
		}

		SDL_FillRect(screen, &rect_in, SDL_MapRGB(screen->format, 143, 103,1));

		/* position of text based on option button dimension */

		dst.x = rect_in.x + ((rect_in.w - title->w) / 2);
		dst.y = rect_in.y + ((rect_in.h - title->h) / 2);
		dst.w = title->w;
		dst.h = title->h;

		SDL_BlitSurface(title, NULL, screen, &dst);
	}
}
