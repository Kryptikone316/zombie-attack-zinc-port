/*
 * l_hud.c
 * See header file for more information about module
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
#include <libintl.h>
#include "s_font.h"
#include "l_hud.h"
#define _(String) gettext (String)

void hud_update_info(hud_t *hud)
{
	char txtbuf[512];
	char *mname;

	if (hud->env->mtime == TIME_PM) mname = _("Day");
	else mname = _("Night");

	sprintf(txtbuf, _("%s: %d, Level: %d, Experience: %d / %d"),
			mname, hud->env->nday,
			hud->player->level,
			hud->player->score,
			hud->player->next_level);

	hud->last_score = hud->player->score;
	hud->last_level = hud->player->level;
	hud->last_nday = hud->env->nday;
	hud->last_mtime = hud->env->mtime;

	if (hud->info_day) SDL_FreeSurface(hud->info_day);
	if (hud->info_night) SDL_FreeSurface(hud->info_night);
	hud->info_day = font_render_text(txtbuf, hud->glyphs_ptr, 0,0,255);
	hud->info_night = font_render_text(txtbuf, hud->glyphs_ptr, 0,255,0);
}

int hud_init(hud_t *hud, env_t *env, player_t *player, SDL_Surface **glyphs_ptr)
{
	hud->player = player;
	hud->env = env;
	hud->glyphs_ptr = glyphs_ptr;

	hud->life_day = font_render_text(_("Life:"), hud->glyphs_ptr, 0, 0, 255);
	hud->gun_day = font_render_text(_("Gun warm:"), hud->glyphs_ptr, 0, 0, 255);
	hud->life_night = font_render_text(_("Life:"), hud->glyphs_ptr, 0, 255, 0);
	hud->gun_night = font_render_text(_("Gun warm:"), hud->glyphs_ptr, 0, 255, 0);
	hud->warm_label = font_render_text(_("Gun warm:"), hud->glyphs_ptr, 255, 0, 0);

	hud->info_day = NULL;
	hud->info_night = NULL;
	hud_update_info(hud);

	return ERR_OK;
}

void hud_clean(hud_t *hud)
{
	SDL_FreeSurface(hud->life_day);
	SDL_FreeSurface(hud->gun_day);
	SDL_FreeSurface(hud->info_day);
	SDL_FreeSurface(hud->life_night);
	SDL_FreeSurface(hud->gun_night);
	SDL_FreeSurface(hud->info_night);
	SDL_FreeSurface(hud->warm_label);
}

void hud_draw(hud_t *hud, SDL_Surface *screen)
{
	SDL_Rect rect;

	/* update top text line if need */
	if (hud->player->score != hud->last_score || hud->player->level != hud->last_level ||
		hud->last_nday != hud->env->nday || hud->last_mtime != hud->env->mtime)
	{
		hud_update_info(hud);
	}

	/* draw top text line */

	rect.x = 640 - hud->info_day->w - 2;
	rect.y = 2;
	rect.w = hud->info_day->w;
	rect.h = hud->info_day->h;
	if (hud->env->mtime == TIME_PM)
		SDL_BlitSurface(hud->info_day, NULL, screen, &rect);
	else
		SDL_BlitSurface(hud->info_night, NULL, screen, &rect);

	/* draw life bar */

	rect.x = 640 - hud->life_day->w - 2 - 200;
	rect.y = hud->info_day->h + 4;
	rect.w = hud->life_day->w;
	rect.h = hud->life_day->h;
	if (hud->env->mtime == TIME_PM)
		SDL_BlitSurface(hud->life_day, NULL, screen, &rect);
	else
		SDL_BlitSurface(hud->life_night, NULL, screen, &rect);

	rect.w = (200 * hud->player->life) / hud->player->life_max;
	rect.h = hud->info_day->h;
	rect.x = 640 - 200 - 4;

	if (hud->env->mtime == TIME_PM)
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 255));
	else
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 255, 0));

	/* draw gun warm bar */

	rect.x = 640 - hud->gun_day->w - 2 - 200;
	rect.y = hud->info_day->h + hud->life_day->h + 6;
	rect.w = hud->gun_day->w;
	rect.h = hud->gun_day->h;

	if (hud->player->gun_overload_time)
		SDL_BlitSurface(hud->warm_label, NULL, screen, &rect);
	else {
		if (hud->env->mtime == TIME_PM)
			SDL_BlitSurface(hud->gun_day, NULL, screen, &rect);
		else
			SDL_BlitSurface(hud->gun_night, NULL, screen, &rect);
	}

	rect.w = (200 * hud->player->gun_warm) / hud->player->gun_warm_max;
	rect.h = hud->gun_day->h;
	rect.x = 640 - 200 - 4;

	if (hud->player->gun_overload_time)
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255,0,0));
	else {
		if (hud->env->mtime == TIME_PM)
			SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 255));
		else
			SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 255, 0));
	}
}
