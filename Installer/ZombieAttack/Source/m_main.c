/*
 * m_main.c
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
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <libintl.h>

#include "m_game.h"
#include "s_resources.h"
#include "e_codes.h"

#define _(String) gettext (String)

void exit_error(int ret)
{
    if (ret == ERR_SDL) {
    	fprintf(stderr, _("Error in SDL: %s\n"), SDL_GetError());
    	exit(1);
    } else if (ret == ERR_ENGINE) {
    	fprintf(stderr, _("Error in engine: %s\n"), err_string);
    	exit(1);
    }
}

int main(int argc, char **argv)
{
	int ret, i;
	game_t game_scene;
	SDL_Surface *screen, *icon;
	sound_t sound;

	printf("Zombie Attack\n");

	/* Process command line arguments */

	for (i = 1; i < argc; i ++) {
		if (!strcmp(argv[i],"-window")) {
			vflags &= ~SDL_FULLSCREEN;
	    } else if (!strcmp(argv[i], "-localedir")) {
	    	i++;
	    	if (i == argc) {
	    		fprintf(stderr, "command line argument error\n");
	    		return 1;
	    	} else {
	    		localedir = argv[i];
	    	}
	    } else if (!strcmp(argv[i], "-datadir")) {
	    	i++;
	    	if (i == argc) {
	    		fprintf(stderr, "command line argument error\n");
	    		return 1;
	    	} else {
	    		datadir = argv[i];
	    	}
	    } else if (!strcmp(argv[i], "-fps")) {
	    	show_fps = 1;
	    } else if (!strcmp(argv[i], "-nosound")) {
	    	is_sound = 0;
	    }
	}

	/* Bind to locale  */

	setlocale (LC_ALL, "");
    bindtextdomain ("zombie", localedir);
    textdomain ("zombie");

	/* setup SDL */

    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, _("Error in SDL: %s\n"), SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    /* Setup video mode */

    screen = SDL_SetVideoMode(640,480, 8, vflags);
    if (!screen) {
        fprintf(stderr, _("Couldn't start video mode: %s\n"), SDL_GetError());
        return 1;
    }

    ret = res_load_bitmap("icon", &icon);
    exit_error(ret);

    SDL_WM_SetCaption(_("Zombie Attack"), NULL);
    SDL_WM_SetIcon(icon, NULL);
    SDL_ShowCursor(SDL_FALSE);

    /* Setup audio loop */

    ret = sound_init(&sound);
    exit_error(ret);

    /* Run game scene */

    game_scene.screen = screen;
    game_scene.sound = &sound;
    ret = game_init(&game_scene);
    exit_error(ret);

    game_play(&game_scene);
    game_clean(&game_scene);

    sound_clean(&sound);
    SDL_FreeSurface(icon);
    SDL_FreeSurface(screen);
	return 0;
}
