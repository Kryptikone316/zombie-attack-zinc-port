/*
 * l_enviroment.c
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

#include "l_enviroment.h"
#include "s_resources.h"
#include "e_codes.h"

void update_rects(env_t *env)
{
	if (env->mtime == TIME_PM) {
		env->sky_rect.y = 0;
		env->sunmoon_rect.y = 0;
	} else {
		env->sky_rect.y = env->sky_rect.h;
		env->sunmoon_rect.y = env->sunmoon_rect.h;
	}
}

int env_init(env_t *env)
{
	int ret;

	ret = res_load_bitmap("farm", &env->farm);
	if (ret != ERR_OK) return ret;

	ret = res_load_bitmap("sky", &env->sky);
	if (ret != ERR_OK) return ret;

	ret = res_load_bitmap("sunmoon", &env->sunmoon);
	if (ret != ERR_OK) return ret;

	SDL_SetColorKey(env->farm, SDL_SRCCOLORKEY, SDL_MapRGB(env->farm->format, 255,0,255));
	SDL_SetColorKey(env->sky, SDL_SRCCOLORKEY, SDL_MapRGB(env->sky->format, 255,0,255));
	SDL_SetColorKey(env->sunmoon, SDL_SRCCOLORKEY, SDL_MapRGB(env->sunmoon->format, 255,0,255));

	env->nday = 1;
	env->time = 0;
	env->mtime = TIME_PM;

	env->sky_rect.x = 0;
	env->sky_rect.w = 640;
	env->sky_rect.h = 205;

	env->sunmoon_rect.x = 0;
	env->sunmoon_rect.w = 151;
	env->sunmoon_rect.h = 151;

	env->sunmoon_pos.w = 151;
	env->sunmoon_pos.h = 151;

	update_rects(env);

	return ERR_OK;
}

void env_clean(env_t *env)
{
	SDL_FreeSurface(env->farm);
	SDL_FreeSurface(env->sky);
	SDL_FreeSurface(env->sunmoon);
}

void env_update(env_t *env, Uint32 time)
{
	Uint32 h, w, t, d;

	env->time += time;
	if (env->time > DAYTIME) {
		if (env->mtime == TIME_AM) {
			env->mtime = TIME_PM;
			env->nday++;
		} else {
			env->mtime = TIME_AM;
		}
		env->time = 0;
		update_rects(env);
	}

	h = 250;
	w = 400;
	t = env->time;
	d = DAYTIME;

	env->sunmoon_pos.x = (w*t) / d - 130;
	env->sunmoon_pos.y = (h*t) / d - 130;
}

void env_draw_bg(env_t *env, SDL_Surface *screen)
{
	SDL_BlitSurface(env->sky, &env->sky_rect, screen, NULL);
	SDL_BlitSurface(env->sunmoon, &env->sunmoon_rect, screen, &env->sunmoon_pos);
	SDL_BlitSurface(env->farm, NULL, screen, NULL);
}
