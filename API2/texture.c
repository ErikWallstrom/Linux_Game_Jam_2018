#include "texture.h"
#include "font.h"
#include "log.h"
#include <SDL2/SDL_image.h>

struct Texture* texture_ctorimage(
	struct Texture* self, 
	const char* path,
	SDL_Renderer* renderer
)
{
	log_assert(self, "is NULL");
	log_assert(path, "is NULL");
	log_assert(renderer, "is NULL");

	self->raw = IMG_LoadTexture(renderer, path);
	if(!self->raw)
	{
		log_error("%s", IMG_GetError());
		self->width = 0;
		self->height = 0;
	}
	else
	{
		SDL_QueryTexture(self->raw, NULL, NULL, &self->width, &self->height);
	}

	return self;
}

struct Texture* texture_ctortext(
	struct Texture* self, 
	struct Font* font, 
	const char* text,
	SDL_Renderer* renderer
)
{
	log_assert(self, "is NULL");
	log_assert(font, "is NULL");
	log_assert(text, "is NULL");
	log_assert(renderer, "is NULL");

	int lineheight = TTF_FontLineSkip(font->raw);
	self->width = 0;
	self->height = lineheight;

	int newwidth = -1;
	for(size_t i = 0; i < strlen(text); i++)
	{
		if(text[i] >= ' ' && text[i] <= '~')
		{
			if(newwidth != -1)
			{
				newwidth += font->atlas[text[i] - ' '].width;
				if(newwidth > self->width)
				{
					self->width = newwidth;
					newwidth = -1;
				}
			}
			else
			{
				self->width += font->atlas[text[i] - ' '].width;
			}
		}
		else if(text[i] == '\t')
		{
			//atlas[0] is space character. Tab = 4 spaces
			if(newwidth != -1)
			{
				newwidth += font->atlas[0].width * 4;
				if(newwidth > self->width)
				{
					self->width = newwidth;
					newwidth = -1;
				}
			}
			else
			{
				self->width += font->atlas[0].width * 4;
			}
		}
		else if(text[i] == '\n')
		{
			self->height += lineheight;
			newwidth = 0;
		}
		else
		{
			log_warning("'%c' is not renderable <%s>", text[i], __func__);
		}
	}

	self->raw = SDL_CreateTexture(
		renderer, 
		font->pixelformat, 
		SDL_TEXTUREACCESS_TARGET,
		self->width,
		self->height
	);

	if(!self->raw)
	{
		log_error("%s", SDL_GetError());
	}

	//NOTE: Line below makes transparency work for two textures on eachother
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetTextureBlendMode(self->raw, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, self->raw);
	SDL_RenderClear(renderer);

	int x = 0;
	int y = 0;

	for(size_t i = 0; i < strlen(text); i++)
	{
		if(text[i] >= ' ' && text[i] <= '~')
		{
			int width = font->atlas[text[i] - ' '].width;
			int height = font->atlas[text[i] - ' '].height;

			SDL_RenderCopy(
				renderer, 
				font->atlas[text[i] - ' '].raw, 
				NULL, 
				&(SDL_Rect){.x = x, .y = y, .w = width, .h = height}
			);

			x += width;
		}
		else if(text[i] == '\t')
		{
			x += font->atlas[0].width * 4;
		}
		else if(text[i] == '\n')
		{
			y += lineheight;
			x = 0;
		}
	}

	SDL_SetRenderTarget(renderer, NULL);
	return self;
}

struct Texture* texture_copy(
	struct Texture* self, 
	struct Texture* dest, 
	SDL_Renderer* renderer
)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");
	log_assert(renderer, "is NULL");

	Uint32 format;
	SDL_QueryTexture(
		self->raw,
		&format, 
		NULL, 
		NULL, 
		NULL
	);

	dest->width = self->width;
	dest->height = self->height;
	dest->raw = SDL_CreateTexture(
		renderer, 
		format, 
		SDL_TEXTUREACCESS_TARGET,
		dest->width,
		dest->height
	);

	if(!dest->raw)
	{
		log_error("%s", SDL_GetError());
	}

	//NOTE: Line below makes transparency work for two textures on eachother
	SDL_SetTextureBlendMode(dest->raw, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, dest->raw);
	SDL_RenderCopy(
		renderer, 
		self->raw,
		&(SDL_Rect){0, 0, dest->width, dest->height},
		&(SDL_Rect){0, 0, dest->width, dest->height}
	);
	SDL_SetRenderTarget(renderer, NULL);
	return dest;
}

void texture_dtor(struct Texture* self)
{
	log_assert(self, "is NULL");
	SDL_DestroyTexture(self->raw);
}

