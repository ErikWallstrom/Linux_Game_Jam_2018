#include "player.h"
#include "map.h"
#include "API2/log.h"

struct Player* player_ctor(struct Player* self, SDL_Renderer* renderer)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	self->animations[PLAYERSTANDING_FRONT] = (struct Animation){
		.srects = vec_ctor(struct Rect, 1) 
	};
	vec_pushback(
		self->animations[PLAYERSTANDING_FRONT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_FRONT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERSTANDING_BACK] = (struct Animation){
		.srects = vec_ctor(struct Rect, 1) 
	};
	vec_pushback(
		self->animations[PLAYERSTANDING_BACK].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_BACK * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERSTANDING_RIGHT] = (struct Animation){
		.srects = vec_ctor(struct Rect, 1) 
	};
	vec_pushback(
		self->animations[PLAYERSTANDING_RIGHT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_RIGHT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERSTANDING_LEFT] = (struct Animation){
		.srects = vec_ctor(struct Rect, 1) 
	};
	vec_pushback(
		self->animations[PLAYERSTANDING_LEFT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_LEFT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERWALKING_DOWN] = (struct Animation){
		.srects = vec_ctor(struct Rect, 2),
		.delay = 300
	};
	vec_pushback(
		self->animations[PLAYERWALKING_DOWN].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_FRONT * PLAYER_WIDTH, PLAYER_HEIGHT}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_DOWN].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_FRONT * PLAYER_WIDTH, PLAYER_HEIGHT * 2}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERWALKING_UP] = (struct Animation){
		.srects = vec_ctor(struct Rect, 2),
		.delay = 300
	};
	vec_pushback(
		self->animations[PLAYERWALKING_UP].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_BACK * PLAYER_WIDTH, PLAYER_HEIGHT}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_UP].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_BACK * PLAYER_WIDTH, PLAYER_HEIGHT * 2}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERWALKING_RIGHT] = (struct Animation){
		.srects = vec_ctor(struct Rect, 4),
		.delay = 200
	};
	vec_pushback(
		self->animations[PLAYERWALKING_RIGHT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_RIGHT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_RIGHT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_RIGHT * PLAYER_WIDTH, PLAYER_HEIGHT}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_RIGHT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_RIGHT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_RIGHT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_RIGHT * PLAYER_WIDTH, PLAYER_HEIGHT * 2}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	self->animations[PLAYERWALKING_LEFT] = (struct Animation){
		.srects = vec_ctor(struct Rect, 1),
		.delay = 200
	};
	vec_pushback(
		self->animations[PLAYERWALKING_LEFT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_LEFT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_LEFT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_LEFT * PLAYER_WIDTH, PLAYER_HEIGHT}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_LEFT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_LEFT * PLAYER_WIDTH, 0}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);
	vec_pushback(
		self->animations[PLAYERWALKING_LEFT].srects, 
		(struct Rect){
			.pos    = {PLAYERSTANDING_LEFT * PLAYER_WIDTH, PLAYER_HEIGHT * 2}, 
			.width  = PLAYER_WIDTH, 
			.height = PLAYER_HEIGHT
		}
	);

	self->rect = (struct Rect){
		.pos    = {200.0, 150.0},
		.width  = PLAYER_WIDTH * PLAYER_SCALE,
		.height = PLAYER_HEIGHT * PLAYER_SCALE
	};

	self->oldpos = self->rect.pos;
	self->velocity = (struct Vec2d){0};
	self->force = (struct Vec2d){0};
	self->renderer = renderer;
	self->selectedanimation = PLAYERSTANDING_FRONT;
	self->hp = 100;

	texture_ctorimage(&self->spritesheet, "resources/sprites.png", renderer);
	return self;
}

void player_update(struct Player* self)
{
	log_assert(self, "is NULL");

	if(self->rect.pos.x < 0)
	{
		self->rect.pos.x = 0;
	}
	if(self->rect.pos.y < 0)
	{
		self->rect.pos.y = 0;
	}
	if(self->rect.pos.x + self->rect.width > MAP_WIDTH)
	{
		self->rect.pos.x = MAP_WIDTH - self->rect.width;
	}
	if(self->rect.pos.y + self->rect.height > MAP_HEIGHT)
	{
		self->rect.pos.y = MAP_HEIGHT - self->rect.height;
	}
}

void player_render(
	struct Player* self, 
	double interpolation, 
	double delta, 
	double camerax, 
	double cameray)
{ 
	/*atan2(
		SDL_GameControllerGetAxis(
			gamestate.input->controller, 
			SDL_CONTROLLER_AXIS_RIGHTY
		), 
		SDL_GameControllerGetAxis(
			gamestate.input->controller, 
			SDL_CONTROLLER_AXIS_RIGHTX
		)
	) * ( 180.0 / M_PI ) + 90.0*/

	log_assert(self, "is NULL");
	animation_update(&self->animations[self->selectedanimation], delta);

	struct Rect srect = self->animations[self->selectedanimation].srects[
		self->animations[self->selectedanimation].frame];

	SDL_RenderCopy(
		self->renderer, 
		self->spritesheet.raw, 
		&(SDL_Rect){
			srect.pos.x,
			srect.pos.y,
			srect.width,
			srect.height
		},
		&(SDL_Rect){
			self->oldpos.x + (self->rect.pos.x - self->oldpos.x) * interpolation
				- camerax,
			self->oldpos.y + (self->rect.pos.y - self->oldpos.y) * interpolation
				- cameray,
			self->rect.width,
			self->rect.height
		}
	);
}

void player_dtor(struct Player* self)
{
	log_assert(self, "is NULL");
	texture_dtor(&self->spritesheet);
	for(size_t i = 0; i < NUM_ANIMATIONS; i++)
	{
		vec_dtor(self->animations[i].srects);
	}
}

