/*
 * s_resources.h
 * This module is used for load external resources data,
 * and for store some information about configuration.
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
#ifndef RESOURCES_H_
#define RESOURCES_H_

#include "SDL.h"
#include "s_audio.h"

extern char *datadir;
extern char *localedir;
extern Uint32 vflags;
extern int show_fps;
extern int is_sound;

int res_load_bitmap(const char *name, SDL_Surface **surface);
int res_load_wav(const char *name, wave_t *wav);
void res_free_wav(wave_t *wav);

#endif /* RESOURCES_H_ */
