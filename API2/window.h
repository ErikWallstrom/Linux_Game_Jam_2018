#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

enum WindowFlags
{
	WINDOW_DEFAULT	 = 0,
	WINDOW_VSYNC 	 = 1 << 0,
	WINDOW_FULLSCREEN = 1 << 1,
};

struct Window
{
	SDL_Window* raw;
	SDL_Renderer* renderer;

	const char* title;
	int width, height;
	int flags;
	unsigned fps, frames, seconds;
};

struct Window* window_ctor(
	struct Window* self, 
	const char* title, 
	int width, 
	int height,
	enum WindowFlags flags
);
void window_render(struct Window* self);
void window_dtor(struct Window* self);

#endif
