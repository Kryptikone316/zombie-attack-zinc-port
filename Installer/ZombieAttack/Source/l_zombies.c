#include "s_resources.h"
#include "l_zombies.h"

#define SPEED_SLOW      75
#define SPEED_MEDIUM    50
#define SPEED_FAST      20

int zombies_factory_init(zombies_factory_t *factory)
{
	int i, ret;

	ret = res_load_wav("zombie_hit", &factory->hit);
	if (ret != ERR_OK) return ret;

	for (i=0; i<8; i++) {
		char name[32];
		sprintf(name, "zombie_sound0%d", i+1);
		ret = res_load_wav(name, &factory->groan[i]);
		if (ret != ERR_OK) return ret;
	}
	ret = res_load_bitmap("zombie", &factory->zombies_surf);
	if (ret != ERR_OK) return ret;
	return ERR_OK;
}

void zombies_factory_clean(zombies_factory_t *factory)
{
	int i;
	for (i=0; i<8; i++) {
		res_free_wav(&factory->groan[i]);
	}
	res_free_wav(&factory->hit);

	SDL_FreeSurface(factory->zombies_surf);
}

int zombies_factory_init_zombie(zombies_factory_t *factory, zombie_t *zombie, int index)
{
	int offy, speed;

	character_init_defaults(&zombie->c);

	offy = index * 146;
	split_to_animation(factory->zombies_surf,zombie->c.walk,DR_LEFT,  48, 73, 0,  offy + 0,  2);
	split_to_animation(factory->zombies_surf,zombie->c.die,DR_LEFT,   84, 73, 0,  offy + 73, 3);

	switch (index) {
	case 0: case 1: case 2:
		speed = SPEED_SLOW;
		zombie->life = 20;
		break;
	}

	zombie->c.direct_look = DR_RIGHT;
	zombie->c.direct_move = DR_RIGHT;
	zombie->c.state = ST_WALK;
	zombie->c.frametime_max = 150;
	zombie->c.movetime_x = rand_range(speed - 10, speed + 10);
	zombie->c.movetime_y = rand_range(speed - 10, speed + 10) + speed / 3;
	zombie->range_go_y = rand_range(0, END_X/2);
	zombie->range_attack = rand_range(2, 7);
	zombie->factory = factory;
	return ERR_OK;
}

void zombie_clean(zombie_t *zombie)
{
	character_clean(&zombie->c);
}

int zombies_buff_add(zombies_buff_t *buff, zombie_t **zombie)
{
	if (buff->zombies_count < MAX_ZOMBIES) {
		if (zombie != NULL)
			*zombie = &buff->zombies[buff->zombies_count];
		buff->zombies_count ++;
		return ERR_OK;
	} else {
		err_string = "Number of zombies take limit";
		return ERR_WARNING;
	}

}

void zombies_buff_remove(zombies_buff_t *buff, int index)
{
	int i;

	zombie_clean(&buff->zombies[index]);

	for (i=index+1; i<buff->zombies_count; i++)
		buff->zombies[i-1] = buff->zombies[i];
	buff->zombies_count --;
}
void zombies_buff_clean(zombies_buff_t *buff)
{
	int i;
	for (i=0;i<buff->zombies_count;i++) {
		zombie_clean(&buff->zombies[i]);
	}
}

void zombie_update(zombie_t *zombie, player_t *player, bullets_buff_t *bulls, sound_t *snd, Uint32 time)
{
	int i;
	Sint16 distx, disty;


	if (zombie->c.state == ST_DYING || zombie->c.state == ST_DIE) {
		character_update(&zombie->c, time);
		return;
	}

	/* sprawdzam czy oberwal */

	for (i=0; i<bulls->bullets_count; i++) {
		bullet_t *bull = &bulls->bullet[i];
		distx = zombie->c.posx - bull->posx;
		disty = zombie->c.posy - bull->posy;
		if (distx < 0) distx *= -1;
		if (disty < 0) disty *= -1;

		if (distx < 4 && disty < 10) {
			zombie->life -= 10 - disty;
			player->score += 10 - disty;

			/* zombie powinien umrzec jesli oberwal wystarczajaco,
			 * lub zareagowac na ostrzal i isc dalej */

			if (zombie->life < 1) {
				zombie->c.state = ST_DYING;
				zombie->c.cur_frame = 0;
				sound_play(snd, CHN_HIT, &zombie->factory->hit);
			} else {
				zombie->hit_time = 70;
				zombie->hitx = bull->posx - 3;
				zombie->hity = bull->posy - 46;
			}

			/* kasuje pocisk */
			bullets_remove(bulls, i);
			i--;
		}
	}

	/* skracam czas postoju spowodowanego uderzeniem pocisku */

	if (zombie->hit_time > 0) {
		zombie->hit_time -= time;
		if (zombie->hit_time < 0) zombie->hit_time = 0;
		return;
	}

	/* ustalam kierunek oraz ruch zombiego */

	distx = zombie->c.posx - player->c.posx;
	disty = zombie->c.posy - player->c.posy;
	if (distx < 0) distx *= -1;
	if (disty < 0) disty *= -1;

	zombie->c.direct_move = 0;

	if (distx > zombie->range_attack) {
		if (player->c.posx < zombie->c.posx) {
			zombie->c.direct_look = DR_LEFT;
			zombie->c.direct_move = DR_LEFT;
		} else if (player->c.posx > zombie->c.posx) {
			zombie->c.direct_look = DR_RIGHT;
			zombie->c.direct_move = DR_RIGHT;
		}
	}

	if (disty > zombie->range_attack && distx < zombie->range_go_y) {
		if (player->c.posy < zombie->c.posy) zombie->c.direct_move |= DR_UP;
		else if (player->c.posy > zombie->c.posy) zombie->c.direct_move |= DR_DOWN;
	}


	character_update(&zombie->c, time);
}

void zombie_draw(zombie_t *zombie, SDL_Surface *screen)
{
	character_draw(&zombie->c, screen);

	if (zombie->hit_time > 0) {
		SDL_Rect rect;
		rect.x = zombie->hitx - 3;
		rect.y = zombie->hity - 3;
		rect.w = rect.h = 5;
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255,0,0));
	}
}
