#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL_ttf.h>
#include "vec.h"

struct Texture;
struct Font
{
	SDL_Color color;
	Vec(struct Texture) atlas;
	TTF_Font* raw;
	size_t size;
	Uint32 pixelformat;
};

struct Font* font_ctor(
	struct Font* self, 
	const char* path, 
	size_t size, 
	SDL_Color color,
	SDL_Renderer* renderer
);
void font_dtor(struct Font* self);

#endif
