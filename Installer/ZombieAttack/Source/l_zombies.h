#ifndef ZOMBIES_H_
#define ZOMBIES_H_

#include <SDL.h>
#include "s_audio.h"
#include "s_character.h"
#include "l_player.h"
#include "e_codes.h"

#define MAX_ZOMBIES 128
#define NUM_ZOMBIES 8

typedef struct {
	SDL_Surface  *zombies_surf;
	wave_t		  groan[8];
	wave_t        hit;
} zombies_factory_t;

typedef struct {
	character_t   c;
	zombies_factory_t *factory;

	Uint16        range_go_y;     /* szerokosc po ktorej zombie zaczyna zblizac sie do gracza na osi y  */
	Uint16        range_attack;   /* odleglosc od gracza od ktorej zombie przestanie scigac gracza      */
	Sint16        life;
	Sint16        hit_time;       /* czas do konca hit-time czyli okresu jak dostal kulke */
	Sint16        hitx, hity;
} zombie_t;



typedef struct {
	zombie_t zombies[MAX_ZOMBIES];
	Sint8    zombies_count;
} zombies_buff_t;

int zombies_factory_init(zombies_factory_t *factory);
void zombies_factory_clean(zombies_factory_t *factory);
int zombies_factory_init_zombie(zombies_factory_t *factory, zombie_t *zombie, int index);

int zombies_buff_add(zombies_buff_t *buff, zombie_t **zombie);
void zombies_buff_remove(zombies_buff_t *buff, int index);
void zombies_buff_clean(zombies_buff_t *buff);

void zombie_clean(zombie_t *zombie);
void zombie_update(zombie_t *zombie, player_t *player, bullets_buff_t *bulls, sound_t *snd, Uint32 time);
void zombie_draw(zombie_t *zombie, SDL_Surface *screen);

#endif /* ZOMBIES_H_ */
