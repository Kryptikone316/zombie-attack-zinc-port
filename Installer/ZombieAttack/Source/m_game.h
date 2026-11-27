#ifndef GAME_H_
#define GAME_H_

#include <SDL.h>
#include "s_audio.h"
#include "s_font.h"
#include "l_enviroment.h"
#include "l_player.h"
#include "l_zombies.h"
#include "l_bullets.h"
#include "l_hud.h"
#include "m_advance.h"
#include "e_codes.h"

typedef enum {
		ERA_DAY,
		ERA_NIGHT
} era_t;

/* time between drop a zombie */
#define ZOMBIE_TIME         2000

/* time between groan should be heared */
#define GROAN_TIME_LIMIT	5000

/* time between surround voices should be heared  */
#define SURROUND_TIME_LIMIT 40000

/* promile of speed up zombie_time after each zombie was drop */
#define GAME_BOOST   4

typedef struct {
	Uint8                gameloop;
	Uint32				 time;
	Uint32				 last_ticks;

	sound_t             *sound;

	SDL_Surface         *screen;
	SDL_Surface         *graveyard;
	SDL_Surface			*score_board;
	SDL_Surface			*fps_info;

	env_t				 env;
	hud_t				 hud;
	player_t             player;
	advance_t			 adv;
	zombies_factory_t    zombies_factory;
	zombies_buff_t       zombies;
	bullets_buff_t       bullets;
	font_t               font;

	Uint16               zombietime_cur;
	Uint16               zombietime_max;

	Uint16				 groan_time;
	Uint16				 surround_time;
	wave_t				 surround[3];
} game_t;

int game_init(game_t *scene);
int game_play(game_t *scene);
void game_clean(game_t *scene);

void game_keydown_event(game_t *game, SDLKey key);
void game_keyup_event(game_t *game, SDLKey key);
void game_update(game_t *game, Uint32 time);
void game_redraw(game_t *game);
void game_pause(game_t *game);
void game_drop_zombie(game_t *game);
void game_play_sound(game_t *game, Uint32 time);

#endif /* GAME_H_ */
