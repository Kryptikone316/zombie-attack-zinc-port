/*
 * l_player.h
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

#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL.h>
#include "s_character.h"
#include "s_audio.h"
#include "l_bullets.h"
#include "e_codes.h"

/* upgrade flags */

#define UPG_GRANADE				1
#define UPG_SPEED_MOVE			2
#define UPG_MORE_SHOOTS			4
#define UPG_FAST_SHOOTS			8
#define UPG_LASER_AIM			16
#define UPG_DIR_SHOOTS			32
#define UPG_TIME_STOP			64
#define	UPG_MORE_LIFE			128
#define UPG_FAST_LIFE_RESTORE	256
#define UPG_MORE_DAMAGE			512

/* battlefield range */

#define BEG_X           20
#define END_X           620
#define BEG_Y           327
#define END_Y           480

/* level of life */

#define PLAYER_LIFE       20
#define PLAYER_LIFE_UPG   40

/* score for next level */
// FIXME: should be 1500
#define PLAYER_NEXT_LEVEL 500

/* speed of player animation in different situations */

#define FS_WALK         100
#define FS_SHOT         90
#define FS_STOP         500

/* maks bullets on screen */

#define MAX_BULLETS     64

/* time between bullets */

#define SHOOT_WAIT       80
#define SHOOT_WAIT_UPG   40

/* gun warm limit (in bullets) */

#define GUN_WARM_DECREASE_TIME		250
#define GUN_OVERLOAD_TIME			2000
#define GUN_WARM_MAX      			16
#define GUN_WARM_MAX_UPG  			32

/* player data */

typedef struct {
	character_t  c;
	Sint16       shoot_wait;  /* time to the next shoot*/
	wave_t       shoot_sound;

	Sint16       gun_warm;
	Sint16		 gun_warm_time;
	Sint16		 gun_warm_max;
	Sint16		 gun_overload_time;

	Sint16		 life;
	Sint16		 life_max;
	Sint16		 life_time;

	Sint16		 score;
	Sint16		 level;
	Sint16		 next_level;
	Uint32		 upg_flags;
} player_t;

int player_init (player_t *player);
void player_clean (player_t *player);

void player_update(player_t *player, bullets_buff_t *bullets, sound_t *snd, Uint32 time);
void player_keydown(player_t *player, SDLKey key);
void player_keyup(player_t *player, SDLKey key);
void player_upgrade(player_t *player, Uint32 upg_flag);

#endif /* PLAYER_H_ */
