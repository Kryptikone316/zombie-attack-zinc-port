#include <string.h>

#include "s_resources.h"
#include "s_font.h"

void extract_chars(SDL_Surface *source, SDL_Surface **font, int offx, int offy, int w, int h, const char *chars)
{
	SDL_Rect rect;
	int i;

	rect.w = w;
	rect.h = h;
	rect.y = offy;
	for (i=0;chars[i] != '\0';i++) {
		SDL_Surface *character = SDL_CreateRGBSurface(SDL_HWSURFACE,w,h,8,0,0,0,0);
		rect.x = offx + i*w;

		SDL_SetPalette(character, SDL_LOGPAL|SDL_PHYSPAL, source->format->palette->colors, 0, source->format->palette->ncolors);
		SDL_BlitSurface(source, &rect, character, NULL);
		SDL_SetColorKey(character, SDL_SRCCOLORKEY, SDL_MapRGB(character->format, 255,0,255));

		font[(int)chars[i]] = character;
	}
}

int font_init(font_t *font)
{
	SDL_Surface *source;
	int ret;

	memset(font, 0, sizeof(font_t));

	ret = res_load_bitmap("font", &source);
	if (ret != ERR_OK) return ret;

	extract_chars(source, font->small, 0, 0,   7, 14, "QWERTYUIOPASDFGHJKLZXCVBNM");
	extract_chars(source, font->small, 0, 14,  7, 14, "qwertyuiopasdfghjklzxcvbnm");
	extract_chars(source, font->small, 0, 28,  7, 14, "1234567890!?()<>,.;:'\"@-=_+/");
	extract_chars(source, font->big,   0, 42,  16, 33, "QWERTYUIOPASDFGHJKLZXCVBNM");
	extract_chars(source, font->big,   0, 75,  16, 33, "qwertyuiopasdfghjklzxcvbnm");
	extract_chars(source, font->big,   0, 108, 16, 33, "1234567890!?()<>,.;:'\"@-=_+/");

	SDL_FreeSurface(source);
	return ERR_OK;
}

void font_clean(font_t *font)
{
	int i;
	for (i=0;i<256;i++) {
		if (font->big[i]) SDL_FreeSurface(font->big[i]);
		if (font->small[i]) SDL_FreeSurface(font->small[i]);
	}
}

SDL_Surface * font_render_text(const char *text, SDL_Surface **font, Uint8 r, Uint8 g, Uint8 b)
{
	SDL_Surface *text_surf;
	SDL_Rect src_rect, dst_rect;
	int i;
	SDL_Palette *pal = NULL;

	/* szukam pierwszego znaku w tej czcionce by pobrac wymiary */
	for (i=0;i<256;i++) {
		if (font[i]) {
			dst_rect.w = src_rect.w = font[i]->w;
			dst_rect.h = src_rect.h = font[i]->h;
			pal = font[i]->format->palette;
			break;
		}
	}

	/* jesli ta czcionka nie zawiera zadnego znaku anuluj operacje */
	if (src_rect.w == 0 || src_rect.h == 0) {
		return NULL;
	}

	/* tworze docelowa powierzchnie */
	text_surf = SDL_CreateRGBSurface(SDL_HWSURFACE, strlen(text)*src_rect.w+1, src_rect.h+1, 8, 0,0,0,0);
	SDL_SetPalette(text_surf, SDL_LOGPAL|SDL_PHYSPAL, pal->colors, 0, pal->ncolors);
	SDL_FillRect(text_surf, NULL, SDL_MapRGB(text_surf->format, 255,0,255));

	/* przyklejam literki */

	src_rect.x = 0;
	src_rect.y = 0;
	for (i=0; text[i] != '\0'; i++) {
		int c = text[i];
		if (font[c]) {
			dst_rect.x = i*src_rect.w;
			dst_rect.y = 0;
			SDL_BlitSurface(font[c], &src_rect, text_surf, &dst_rect);
		}
	}

	/* ustalam odpowiedni kolor literek */
	text_surf->format->palette->colors[1].r = r;
	text_surf->format->palette->colors[1].g = g;
	text_surf->format->palette->colors[1].b = b;
	SDL_SetColorKey(text_surf, SDL_SRCCOLORKEY, SDL_MapRGB(text_surf->format, 255,0,255));
	return text_surf;
}
