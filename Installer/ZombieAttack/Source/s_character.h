/*
 *  Character is object based for zombies and player. Implements of animation,
 *  position and state, after initializing character could be draw or update.
 */

#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <SDL.h>
#include <stdlib.h>
#include "e_codes.h"

#define rand_range(min,max) ((rand() % ((max) - (min))) + (min))

typedef enum {
	ST_STOP,
	ST_WALK,
	ST_SHOT,
	ST_DYING,
	ST_DIE
} state_t;

typedef enum {
	DR_LEFT = 1,
	DR_RIGHT = 2,
	DR_UP = 4,
	DR_DOWN = 8
} direction_t;

/*
 * Maximum frames in animation, tables of animations are MAX_FRAMES * 2 because
 * from 0 to 31 is left direct animations, and 32 to 63 is right direct animation,
 * for calculate see makro above.
 */

#define MAX_FRAMES 32
#define CALC_FRAME(direction, frame) direction == DR_LEFT ? frame : MAX_FRAMES+frame

/******************************************************************************
 *                                    Character
 *****************************************************************************/


typedef struct {
	Sint16 posx, posy;

	/* *****************************************
	 *            animation variants
	 * ****************************************/

	SDL_Surface *stop[MAX_FRAMES*2];
	SDL_Surface *walk[MAX_FRAMES*2];
	SDL_Surface *shot[MAX_FRAMES*2];
	SDL_Surface *die[MAX_FRAMES*2];

	/* ***************************************
	 *            state of character
	 * **************************************/
	state_t     state;

	Uint16 frametime_max;       /* max time of frame show                    */
	Sint16 frametime_cur;       /* show time of current frame                */

	Uint8       cur_frame;      /* current frame number                      */
	Uint16      movetime_x;     /* time of 1 pixel move                      */
	Uint16      movetime_y;
	Sint16      curtime_x;      /* time after last move                      */
	Sint16      curtime_y;


	direction_t direct_look;    /* look direct                               */
	direction_t direct_move;    /* move direct, could be mixed of flags
	                               (DR_LEFT | DR_UP)                         */
} character_t;

/* usefull for preparing frames, see example of usage to understand */

void
split_to_animation(SDL_Surface *from, SDL_Surface **film, direction_t direction,
		  int width, int height, int offset_x, int offset_y, int steps);

/* manipulation of character */

void character_init_defaults(character_t *c);
void character_draw(character_t *c, SDL_Surface *screen);
void character_update(character_t *c, Uint32 time);
void character_clean(character_t *c);

#endif /* CHARACTER_H_ */
