/*
 * s_resources.c
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
#include <stdio.h>
#include <unistd.h>
#include <zlib.h>
#include <libintl.h>
#include "s_resources.h"

#define _(String) gettext (String)

char *datadir = "./";
char *localedir = "./";
Uint32 vflags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
int show_fps = 0;
int is_sound = 1;

int decompress(const char *fgz_path, char *dst_path)
{
	char buffer[1024];
	FILE *tmp_file;
	gzFile fgz_file;

	/* use of tmpnam must be changed, but is ok for now */

	tmpnam(dst_path);

	/* open compressed and temporary file */

	tmp_file = fopen(dst_path, "wb");
	fgz_file = gzopen(fgz_path, "rb");

	if (!tmp_file || !fgz_file) {
		err_string = _("Decompress file error");
		return ERR_ENGINE;
	}

	/* decompress data to temporary file */

	do {
		int len = gzread(fgz_file, buffer, 1024);
		fwrite(buffer, 1, len, tmp_file);
	} while (!gzeof(fgz_file));

	/* flush data */

	gzclose(fgz_file);
	fclose (tmp_file);
	return ERR_OK;
}

int res_load_bitmap(const char *name, SDL_Surface **surface)
{
	FILE *file;
	int ret;
	char fullpath[512], uncpath[L_tmpnam];

	/* first try to load uncompressed bitmap */

	sprintf(fullpath, "%s/%s.bmp", datadir, name);
	file = fopen(fullpath, "rb");

	if (file) {

		fclose (file);
		*surface = SDL_LoadBMP(fullpath);
		return ERR_OK;

	} else {

		/* if uncompressed file doesn't exist,
		 * check if gzip file is available, and load it */

		sprintf(fullpath, "%s/%s.zbm", datadir, name);
		ret = decompress(fullpath, uncpath);
		if (ret != ERR_OK) return ret;

		*surface = SDL_LoadBMP(uncpath);
		if (!surface) {
			unlink(uncpath);
			return ERR_SDL;
		} else {
			unlink(uncpath);
			return ERR_OK;
		}

	}
}

int res_load_wav(const char *name, wave_t *wav)
{
	SDL_AudioSpec spec;
	FILE *file;
	int ret;
	char fullpath[512], uncpath[L_tmpnam];

	sprintf(fullpath, "%s/%s.wav", datadir, name);
	file = fopen(fullpath, "rb");

	if (file) {

		if (!SDL_LoadWAV(fullpath, &spec, &wav->data, &wav->len)) {
			return ERR_SDL;
		} else {
			return ERR_OK;
		}

	} else {

		sprintf(fullpath, "%s/%s.zwv", datadir, name);
		ret = decompress(fullpath, uncpath);
		if (ret != ERR_OK) return ret;

		if (!SDL_LoadWAV(uncpath, &spec, &wav->data, &wav->len)) {
			unlink(uncpath);
			return ERR_SDL;
		} else {
			unlink(uncpath);
			return ERR_OK;
		}
	}
}

void res_free_wav(wave_t *wav)
{
	SDL_FreeWAV(wav->data);
}
