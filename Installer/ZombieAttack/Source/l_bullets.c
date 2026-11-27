#include "s_resources.h"
#include "l_bullets.h"

int bullets_init(bullets_buff_t *bullets)
{
	SDL_Rect src;
	SDL_Surface *source;
	int ret;

	bullets->bullets_count = 0;
	bullets->curtime =0 ;

	ret = res_load_bitmap("bullet", &source);
	if (ret != ERR_OK) return ret;

	/* tworze bufory do przechowywania wizerunku pocisku */
	bullets->left = SDL_CreateRGBSurface(SDL_HWSURFACE, 6, 3, 8, 0,0,0,0);
	if (!bullets->left) {
		SDL_FreeSurface(source);
		return ERR_SDL;
	}

	bullets->right = SDL_CreateRGBSurface(SDL_HWSURFACE, 6, 3, 8, 0,0,0,0);
	if (!bullets->right) {
		SDL_FreeSurface(source);
		SDL_FreeSurface(bullets->left);
		return ERR_SDL;
	}
	SDL_SetPalette(bullets->left, SDL_LOGPAL|SDL_PHYSPAL, source->format->palette->colors, 0, source->format->palette->ncolors);
	SDL_SetPalette(bullets->right, SDL_LOGPAL|SDL_PHYSPAL, source->format->palette->colors, 0, source->format->palette->ncolors);
	SDL_SetColorKey(bullets->left, SDL_SRCCOLORKEY, SDL_MapRGB(bullets->left->format, 255,0,255));
	SDL_SetColorKey(bullets->right, SDL_SRCCOLORKEY, SDL_MapRGB(bullets->right->format, 255,0,255));

	/* kopiuje wizerunek pocisko do buforow */
	src.x = 0;
	src.y = 0;
	src.w = 6;
	src.h = 3;
	SDL_BlitSurface(source, &src, bullets->right, NULL);

	src.x = 6;
	SDL_BlitSurface(source, &src, bullets->left, NULL);

	SDL_FreeSurface(source);
	return ERR_OK;
}

void bullets_clean(bullets_buff_t *bullets)
{
	SDL_FreeSurface(bullets->left);
	SDL_FreeSurface(bullets->right);
}

void bullets_add(bullets_buff_t *bullets, Uint16 posx, Uint16 posy, direction_t direct)
{
	bullet_t *bullet = &bullets->bullet[bullets->bullets_count];
	bullets->bullets_count++;
	bullet->direct = direct;
	bullet->posx = posx;
	bullet->posy = posy;
}

void bullets_remove(bullets_buff_t *bullets, int index)
{
	int i;
	for (i=index+1; i<bullets->bullets_count; i++)
		bullets->bullet[i-1] = bullets->bullet[i];
	bullets->bullets_count --;
}

void bullets_update(bullets_buff_t *bullets, Uint32 time)
{
	int i;
	bullets->curtime += time;

	while (bullets->curtime > BULLETS_MAX_TIME) {

		bullets->curtime -= BULLETS_MAX_TIME;

		for (i=0;i<bullets->bullets_count;i++) {
			bullet_t *bullet = &bullets->bullet[i];

			if (bullet->direct == DR_RIGHT) {
				bullet->posx +=2;
				if (bullet->posx > 640) {
					bullets_remove(bullets, i);
					i--;
				}
			} else {
				bullet->posx -=2;
				if (bullet->posx < 0) {
					bullets_remove(bullets, i);
					i--;
				}
			}
		}
	}
}

void bullets_draw(bullets_buff_t *bullets, SDL_Surface *screen)
{
	SDL_Surface *bull_surf;
	SDL_Rect dst;
	dst.w = 6;
	dst.h = 3;

	int i;
	for (i=0;i<bullets->bullets_count;i++) {
		bullet_t *bullet = &bullets->bullet[i];
		if (bullet->direct == DR_RIGHT) bull_surf = bullets->right;
		else bull_surf = bullets->left;

		dst.x = bullet->posx - 3;
		dst.y = bullet->posy - 46;
		SDL_BlitSurface(bull_surf, NULL, screen, &dst);
	}
}
