#include "font.h"
#include "texture.h"
#include "log.h"

struct Font* font_ctor(
	struct Font* self, 
	const char* path, 
	size_t size, 
	SDL_Color color,
	SDL_Renderer* renderer
)
{
	log_assert(self, "is NULL");
	log_assert(path, "is NULL");
	log_assert(size > 0, "got invalid size");

	self->raw = TTF_OpenFont(path, size);
	if(!self->raw)
	{
		log_error("%s", TTF_GetError());
	}

	//XXX: Is + 1 really needed? Not sure, math...
	self->atlas = vec_ctor(struct Texture, '~' - ' ' + 1); 
	self->size = size;

	//Every visible character in ascii table
	for(char i = ' '; i <= '~'; i++) 
	{
		if(TTF_GlyphIsProvided(self->raw, i))
		{
			SDL_Surface* surface = TTF_RenderGlyph_Blended(self->raw, i, color);
			if(!surface)
			{
				log_error("%s", TTF_GetError());
			}

			struct Texture texture;
			texture.raw = SDL_CreateTextureFromSurface(renderer, surface);
			if(!texture.raw)
			{
				log_error("%s", SDL_GetError());
			}

			SDL_QueryTexture(
				texture.raw, 
				&self->pixelformat, 
				NULL, &texture.width, 
				&texture.height
			);
			vec_pushback(self->atlas, texture);
			SDL_FreeSurface(surface);
		}
		else
		{
			log_warning("'%c' does not exists in font <%s>", i, __func__);
			//Push empty texture to allow easy access to glyphs when rendering
			vec_pushback(self->atlas, (struct Texture){0});
		}
	}

	return self;
}

void font_dtor(struct Font* self)
{
	log_assert(self, "is NULL");
	for(size_t i = 0; i < vec_getsize(self->atlas); i++)
	{
		SDL_DestroyTexture(self->atlas[i].raw);
	}

	vec_dtor(self->atlas);
	TTF_CloseFont(self->raw);
}

