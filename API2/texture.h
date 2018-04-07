#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

//Forward declartion since font.h includes this file
struct Font;

struct Texture
{
	SDL_Texture* raw;
	int width, height;
};

struct Texture* texture_ctorimage(
	struct Texture* self, 
	const char* path,
	SDL_Renderer* renderer
);
struct Texture* texture_ctortext(
	struct Texture* self, 
	struct Font* font, 
	const char* text,
	SDL_Renderer* renderer
);
struct Texture* texture_copy(
	struct Texture* self, 
	struct Texture* dest, 
	SDL_Renderer* renderer
);
void texture_dtor(struct Texture* self);

#endif
