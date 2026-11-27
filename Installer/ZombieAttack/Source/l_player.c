/*
 * l_player.c
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

#include <stdlib.h>
#include <string.h>
#include "l_player.h"
#include "s_resources.h"

int player_init(player_t *player)
{
	SDL_Surface *commando;
	int ret;

	ret = res_load_wav("gun", &player->shoot_sound);
	if (ret != ERR_OK) return ret;

	ret = res_load_bitmap("commando", &commando);
	if (ret != ERR_OK) return ret;

	character_init_defaults(&player->c);
	split_to_animation(commando,player->c.walk,DR_RIGHT, 48,73,0,0,8);
	split_to_animation(commando,player->c.stop,DR_RIGHT, 48,73,0,73,3);
	split_to_animation(commando,player->c.shot,DR_RIGHT, 57,73,0,146,2);
	SDL_FreeSurface(commando);


	player->score = 0;
	player->level = 1;
	player->next_level = PLAYER_NEXT_LEVEL;

	player->shoot_wait = 0;
	player->gun_warm = 0;
	player->gun_warm_time = 0;
	player->gun_overload_time = 0;
	player->gun_warm_max = GUN_WARM_MAX;

	player->life = PLAYER_LIFE;
	player->life_max = PLAYER_LIFE;
	player->life_time = 0;

	player->c.state = ST_STOP;
	player->c.frametime_max = FS_STOP;
	player->c.movetime_x = 10;
	player->c.movetime_y = 10;
	return ERR_OK;
}

void player_clean (player_t *player)
{
	res_free_wav(&player->shoot_sound);
	character_clean(&player->c);
}

void player_update(player_t *player, bullets_buff_t *bullets, sound_t *snd, Uint32 time)
{
	/* decrease warm level */
	if (player->gun_overload_time) {
		player->gun_overload_time -= time;
		if (player->gun_overload_time < 0) player->gun_overload_time = 0;
	} else {
		player->gun_warm_time += time;
		if (player->gun_warm_time > GUN_WARM_DECREASE_TIME) {
			player->gun_warm_time -= GUN_WARM_DECREASE_TIME;
			if (player->gun_warm > 0) player->gun_warm --;
		}
	}

	switch (player->c.state) {
	case ST_STOP:
		player->c.frametime_max = FS_STOP;
		break;
	case ST_WALK:
		player->c.frametime_max = FS_WALK;
		break;
	case ST_SHOT:

		player->c.frametime_max = FS_SHOT;
		player->shoot_wait -= time;
		if (player->shoot_wait <= 0 && !player->gun_overload_time) {
			if (player->gun_warm == player->gun_warm_max) {
				player->gun_overload_time = GUN_OVERLOAD_TIME;
				/* switch to player stop */
				player->c.frametime_max = FS_STOP;
				player->c.state = ST_STOP;
			} else {
				int offx;
				player->shoot_wait = SHOOT_WAIT;
				player->gun_warm ++;
				if (player->c.direct_look == DR_LEFT) offx = -20; else offx = 20;
				bullets_add(bullets, player->c.posx + offx, player->c.posy, player->c.direct_look);
			}
		}
		if (!sound_isplay(snd, CHN_PLAYER) && !player->gun_overload_time) {
			sound_play(snd, CHN_PLAYER, &player->shoot_sound);
		}
		break;
	case ST_DYING:
		player->c.frametime_max = FS_WALK;
		break;
	case ST_DIE:
		player->c.frametime_max = FS_STOP;
		break;
	}

	character_update(&player->c, time);

	if (player->c.posx < BEG_X) player->c.posx = BEG_X;
	if (player->c.posx > END_X) player->c.posx = END_X;
	if (player->c.posy < BEG_Y) player->c.posy = BEG_Y;
	if (player->c.posy > END_Y) player->c.posy = END_Y;

	if (player->c.state != ST_SHOT) {
		if (sound_isplay(snd, CHN_PLAYER)) sound_stop(snd, CHN_PLAYER);
	}
}

void player_keydown(player_t *player, SDLKey key)
{
	switch (key) {
		case SDLK_LCTRL:
			if (!player->gun_overload_time) {
				player->c.frametime_max = FS_SHOT;
				player->c.state = ST_SHOT;
				player->shoot_wait = 0;
			}
			break;
		case SDLK_LEFT:
			player->c.direct_look = DR_LEFT;
			player->c.direct_move |= DR_LEFT;
			if (player->c.state != ST_SHOT) player->c.state = ST_WALK;
			break;
		case SDLK_RIGHT:
			player->c.direct_look = DR_RIGHT;
			player->c.direct_move |= DR_RIGHT;
			if (player->c.state != ST_SHOT) player->c.state = ST_WALK;
			break;
		case SDLK_UP:
			player->c.direct_move |= DR_UP;
			if (player->c.state != ST_SHOT) player->c.state = ST_WALK;
			break;
		case SDLK_DOWN:
			player->c.direct_move |= DR_DOWN;
			if (player->c.state != ST_SHOT) player->c.state = ST_WALK;
			break;
		default: break;
	}
}

void player_keyup(player_t *player, SDLKey key)
{
	switch (key) {
		case SDLK_LCTRL:
			if (player->c.direct_move) player->c.state = ST_WALK;
			else player->c.state = ST_STOP;
			break;
		case SDLK_LEFT:
			player->c.direct_move &= ~DR_LEFT;
			if (player->c.state == ST_WALK && !player->c.direct_move) player->c.state = ST_STOP;
			break;
		case SDLK_RIGHT:
			player->c.direct_move &= ~DR_RIGHT;
			if (player->c.state == ST_WALK && !player->c.direct_move) player->c.state = ST_STOP;
			break;
		case SDLK_UP:
			player->c.direct_move &= ~DR_UP;
			if (player->c.state == ST_WALK && !player->c.direct_move) player->c.state = ST_STOP;
			break;
		case SDLK_DOWN:
			player->c.direct_move &= ~DR_DOWN;
			if (player->c.state == ST_WALK && !player->c.direct_move) player->c.state = ST_STOP;
			break;
		default: break;
	}
}

void player_upgrade(player_t *player, Uint32 upg_flag)
{
	player->level++;
	player->next_level *= 2;
	player->upg_flags |= upg_flag;
}

