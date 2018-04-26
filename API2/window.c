#include "window.h"
#include "log.h"

struct Window* window_ctor(
	struct Window* self, 
	const char* title, 
	int width, 
	int height,
	enum WindowFlags flags
)
{
	log_assert(self, "is NULL");
	log_assert(title, "is NULL");
	log_assert(width > 0, "invalid width");
	log_assert(height > 0, "invalid height");

	self->raw = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		(flags & WINDOW_FULLSCREEN) ? 
			SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN
	);
	if(!self->raw)
	{
		log_error("%s", SDL_GetError());
	}
	
	int renderflags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if(flags & WINDOW_VSYNC)
	{
		renderflags |= SDL_RENDERER_PRESENTVSYNC;
	}

	self->renderer = SDL_CreateRenderer(self->raw, -1, renderflags);
	if(!self->renderer)
	{
		log_error("%s", SDL_GetError());
	}

	SDL_RenderSetLogicalSize(self->renderer, width, height);

	self->title = SDL_GetWindowTitle(self->raw);
	self->flags = flags;
	self->width = width;
	self->height = height;
	self->fps = 0;
	self->frames = 0;
	self->seconds = SDL_GetTicks() / 1000;

	return self;
}

void window_render(struct Window* self)
{
	log_assert(self, "is NULL");
	SDL_RenderPresent(self->renderer);
	SDL_RenderClear(self->renderer);

	self->frames++;
	if(SDL_GetTicks() / 1000 > self->seconds)
	{
		self->seconds = SDL_GetTicks() / 1000;
		self->fps = self->frames;
		self->frames = 0;
	}

	if(SDL_GetWindowFlags(self->raw) & SDL_WINDOW_MINIMIZED)
	{
		SDL_Delay(1000 / 60); //Limit framerate while minimized (fix SDL bug)
	}
}

void window_dtor(struct Window* self)
{
	log_assert(self, "is NULL");

	SDL_DestroyRenderer(self->renderer);
	SDL_DestroyWindow(self->raw);
}

