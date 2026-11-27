#include <string.h>
#include <libintl.h>

#include "s_resources.h"
#include "m_game.h"

#define _(String) gettext (String)

int game_init(game_t *game)
{
	int ret;

	srand(SDL_GetTicks());

    /* create player object */
    ret = player_init(&game->player);
    if (ret != ERR_OK) return ret;

    /* buffer for bullets */
    ret = bullets_init(&game->bullets);
    if (ret != ERR_OK) return ret;

    /* factory of zombies */
    ret = zombies_factory_init(&game->zombies_factory);
    if (ret != ERR_OK) return ret;

    /* initiate zombies buffor */
    memset(&game->zombies, 0, sizeof(zombies_buff_t));

    /* read font */
    ret = font_init(&game->font);
    if (ret != ERR_OK) return ret;

	/* create enviroment */
    ret = env_init(&game->env);
    if (ret != ERR_OK) return ret;

    /* create hud object */
    ret = hud_init(&game->hud, &game->env, &game->player, game->font.small);
    if (ret != ERR_OK) return ret;

    /* create advance menu */
	advance_init(&game->adv, &game->font);

    /* graveyard for stored dead bodies */
    game->graveyard = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, 8, 0,0,0,0);
    SDL_SetPalette(game->graveyard, SDL_LOGPAL|SDL_PHYSPAL,
    		game->zombies_factory.zombies_surf->format->palette->colors, 0,
    		game->zombies_factory.zombies_surf->format->palette->ncolors);
    SDL_FillRect(game->graveyard, NULL, SDL_MapRGB(game->graveyard->format, 255,0,255));
    SDL_SetColorKey(game->graveyard, SDL_SRCCOLORKEY, SDL_MapRGB(game->graveyard->format, 255,0,255));


    /* load sound effects */
   	ret = res_load_wav("surround01", &game->surround[0]);
   	if (ret != ERR_OK) return ret;
    ret = res_load_wav("surround02", &game->surround[1]);
    if (ret != ERR_OK) return ret;
    ret = res_load_wav("surround03", &game->surround[2]);
    if (ret != ERR_OK) return ret;

    game->zombietime_max = ZOMBIE_TIME;
    game->zombietime_cur = 0;
    game->fps_info = NULL;
    game->groan_time = 0;
    game->surround_time = SURROUND_TIME_LIMIT;

    return ERR_OK;
}

void game_clean(game_t *game)
{
	SDL_FreeSurface(game->graveyard);

	res_free_wav(&game->surround[0]);
	res_free_wav(&game->surround[1]);
	res_free_wav(&game->surround[2]);

	env_clean(&game->env);
	hud_clean(&game->hud);
	advance_clean(&game->adv);
	font_clean(&game->font);
	bullets_clean(&game->bullets);
	zombies_buff_clean(&game->zombies);
	zombies_factory_clean(&game->zombies_factory);
	player_clean(&game->player);
}

int next_srand = 0;
int game_play(game_t *game)
{
	SDL_Event event;
	Uint32 updatetime;
	int framerate = 0;

	game->last_ticks = SDL_GetTicks();
	updatetime = game->last_ticks;

	game->gameloop = 1;
	while (game->gameloop) {

		/* time data actualisation */

		Uint32 ticks = SDL_GetTicks();
		game->time = ticks - game->last_ticks;
		game->last_ticks = ticks;

		/* after 700 ms reset random numbers to new value */

		if (next_srand < 0) {
			srand(SDL_GetTicks());
			next_srand = 700;
		} else next_srand -= game->time;

		/* process events from system */

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN:
				game_keydown_event(game, event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				game_keyup_event(game, event.key.keysym.sym);
				break;
			case SDL_QUIT:
				game->gameloop = 0;
			    break;
			}
		}

		/* check advance player */
		if (game->player.score > game->player.next_level) {
			player_upgrade(&game->player, advance_choose(&game->adv, &game->player, game->screen));
			game->time = 0;
			game->last_ticks = SDL_GetTicks();
		}

		/* update game objects */

		game_play_sound(game, game->time);
		game_update(game, game->time);

		/* draw game objects */
		game_redraw(game);
		SDL_Flip(game->screen);

		/* show fps is needed */

		if (show_fps) {
			framerate++;
			if (ticks > updatetime + 1000) {
				char text[64];

				sprintf(text, "FPS:%3d", framerate);
				if (game->fps_info) SDL_FreeSurface(game->fps_info);
				game->fps_info = font_render_text(text, game->font.small, 0,0,0);
				framerate = 0;
				updatetime += 1000;
			}
		}

	}

	return ERR_OK;
}

void game_play_sound(game_t *game, Uint32 time)
{
	game->groan_time += time;
	if (game->groan_time > GROAN_TIME_LIMIT) {
		if (!sound_isplay(game->sound, CHN_ZOMBIE))
			sound_play(game->sound, CHN_ZOMBIE, &game->zombies_factory.groan[rand_range(0,8)]);
		game->groan_time = 0;
	}

	game->surround_time += time;
	if (game->surround_time > SURROUND_TIME_LIMIT) {
		sound_play(game->sound, CHN_SURROUND, &game->surround[rand_range(0,3)]);
		game->surround_time = 0;
	}
}

void game_update(game_t *game, Uint32 time)
{
	int i;

	env_update(&game->env, time);
	player_update(&game->player, &game->bullets, game->sound, time);
	bullets_update(&game->bullets, time);

	game->zombietime_cur += time;
	if (game->zombietime_cur > game->zombietime_max) {
		game->zombietime_cur -= game->zombietime_max;
		game_drop_zombie(game);
		game->zombietime_max -= ((float)game->zombietime_max/1000)*GAME_BOOST;
	}

	for (i=0; i<game->zombies.zombies_count; i++) {
		zombie_t *zombie = &game->zombies.zombies[i];
		if (zombie->c.state == ST_DIE) {
			zombie_draw(zombie, game->graveyard);
			zombies_buff_remove(&game->zombies, i);
			i--;
		} else {
			zombie_update(zombie, &game->player, &game->bullets, game->sound, time);
		}
	}
}

void game_redraw(game_t *game)
{
	int i;

	env_draw_bg(&game->env, game->screen);

	SDL_BlitSurface(game->graveyard, NULL, game->screen, NULL);

	/* rysuj zombie powyzej gracza */
	for (i=0; i<game->zombies.zombies_count; i++) {
		zombie_t *z =  &game->zombies.zombies[i];
		if (z->c.posy < game->player.c.posy)
			zombie_draw(z, game->screen);
	}

	character_draw(&game->player.c, game->screen);


	/* rysuj zombie ponizej gracza */
	for (i=0; i<game->zombies.zombies_count; i++) {
		zombie_t *z =  &game->zombies.zombies[i];
		if (z->c.posy >= game->player.c.posy)
			zombie_draw(z, game->screen);
	}

	bullets_draw(&game->bullets, game->screen);

	if (game->fps_info) {
		SDL_Rect rect;
		rect.x = 640 - game->fps_info->w - 4;
		rect.y = 480 - game->fps_info->h - 4;
		rect.w = game->fps_info->w;
		rect.h = game->fps_info->h;
		SDL_BlitSurface(game->fps_info, NULL, game->screen, &rect);
	}

	hud_draw(&game->hud, game->screen);
}

void game_keydown_event(game_t *game, SDLKey key)
{
	if (key == SDLK_ESCAPE) {
		game->gameloop = 0;
	} else if (key == SDLK_PAUSE || key == SDLK_p) {
		game_pause(game);
	} else {
		player_keydown(&game->player, key);
	}
}

void game_keyup_event(game_t *game, SDLKey key)
{
	player_keyup(&game->player, key);
}

void game_pause(game_t *game)
{
	SDL_Event event;
	SDL_Rect dst;

	SDL_Surface *pause = font_render_text(_("Pause game"), game->font.big, 0,0,255);

	dst.x = 0;
	dst.y = (480 - pause->h) / 2;
	dst.w = 640;
	dst.h = pause->h;
	SDL_FillRect(game->screen, &dst, SDL_MapRGB(game->screen->format, 157, 158,156));

	dst.x = (640 - pause->w) / 2;
	dst.y = (480 - pause->h) / 2;
	dst.w = pause->w;
	dst.h = pause->h;

	SDL_BlitSurface(pause, NULL, game->screen, &dst);
	SDL_Flip(game->screen);
	SDL_FreeSurface(pause);

	while(1) {
		SDL_WaitEvent(&event);
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_PAUSE || event.key.keysym.sym == SDLK_p) {
				game->time = 0;
				game->last_ticks = SDL_GetTicks();
				return;
			}
		}
	}
}

void game_drop_zombie(game_t *game)
{
	Uint16 height;
	Sint16 posx, posy;
	zombie_t *zombie;

	height = END_Y - BEG_Y;
	posy = rand_range(0, height * 2 - 1);
	if (posy < height) {
		posx = BEG_X-40;
	} else {
		posy -= height;
		posx = END_X+40;
	}
	posy += BEG_Y;

	zombies_buff_add(&game->zombies, &zombie);
	zombies_factory_init_zombie(&game->zombies_factory, zombie, rand_range(0,2));
	zombie->c.posx = posx;
	zombie->c.posy = posy;

}
