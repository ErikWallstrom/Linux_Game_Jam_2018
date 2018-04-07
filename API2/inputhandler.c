#include "inputhandler.h"
#include "log.h"

struct InputHandler* inputhandler_ctor(struct InputHandler* self)
{
	log_assert(self, "is NULL");

	//TODO: Change 0 to a more appropriate value
	self->events = vec_ctor(SDL_Event, 0); 
	self->keystate = SDL_GetKeyboardState(NULL);
	self->mousestate = SDL_GetMouseState(&self->mousex, &self->mousey);

	if(SDL_NumJoysticks() && SDL_IsGameController(0)) //Uses the first one
	{
		self->controller = SDL_GameControllerOpen(0);
	}
	else
	{
		self->controller = NULL;
		log_warning("No compatible game controllers found");
	}

	return self;
}

void inputhandler_update(struct InputHandler* self)
{
	log_assert(self, "is NULL");
	vec_clear(self->events);

	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		vec_pushback(self->events, event);
	}

	self->mousestate = SDL_GetMouseState(&self->mousex, &self->mousey);
}

void inputhandler_dtor(struct InputHandler* self)
{
	log_assert(self, "is NULL");

	if(self->controller)
	{
		SDL_GameControllerClose(self->controller);
	}

	vec_dtor(self->events);
}

