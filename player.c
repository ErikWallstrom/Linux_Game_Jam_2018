#include "player.h"
#include "map.h"
#include "API2/log.h"

extern const double TICK_RATE; //XXX: hacky

struct Player* player_ctor(
	struct Player* self, 
	struct Vec2d pos, 
	SDL_Renderer* renderer)
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

	rect_ctor(
		&self->rect, 
		pos, 
		RECTREGPOINT_CENTER, 
		PLAYER_WIDTH * PLAYER_SCALE,
		PLAYER_HEIGHT * PLAYER_SCALE
	);

	self->oldpos = self->rect.pos;
	self->direction = (struct Vec2d){0};
	self->force = (struct Vec2d){0};
	self->renderer = renderer;
	self->selectedanimation = PLAYERSTANDING_FRONT;
	self->hp = 100;

	texture_ctorimage(&self->spritesheet, "resources/sprites.png", renderer);
	return self;
}

void player_update(struct Player* self, struct Map* map)
{
	log_assert(self, "is NULL");

	struct Vec2d olddirection = self->direction;
	self->oldpos = self->rect.pos;
	vec2d_scale(&self->direction, PLAYER_SPEED / TICK_RATE, &self->direction);

	if(self->force.x == 0.0)
	{
		self->rect.pos.x += self->direction.x;
	}
	else
	{
		self->rect.pos.x += self->force.x;
	}
	for(size_t i = 0; i < vec_getsize(map->tiles); i++)
	{
		if(rect_intersects(&self->rect, &map->tiles[i].rect))
		{
			struct Vec2d pos1 = rect_getpos(
				&self->rect, 
				RECTREGPOINT_CENTER
			);

			struct Vec2d pos2 = rect_getpos(
				&map->tiles[i].rect, 
				RECTREGPOINT_CENTER
			);

			if(pos1.x < pos2.x)
			{
				self->rect.pos.x = map->tiles[i].rect.pos.x - self->rect.width;
			}
			else
			{
				self->rect.pos.x = map->tiles[i].rect.pos.x 
					+ map->tiles[i].rect.width;
			}

			self->force.x = 0.0;
			//self->direction.x = 0.0;
		}
	}

	if(self->force.y == 0.0)
	{
		self->rect.pos.y += self->direction.y;
	}
	else
	{
		self->rect.pos.y += self->force.y;
	}

	for(size_t i = 0; i < vec_getsize(map->tiles); i++)
	{
		if(rect_intersects(&self->rect, &map->tiles[i].rect))
		{
			struct Vec2d pos1 = rect_getpos(
				&self->rect, 
				RECTREGPOINT_CENTER
			);

			struct Vec2d pos2 = rect_getpos(
				&map->tiles[i].rect, 
				RECTREGPOINT_CENTER
			);

			if(pos1.y < pos2.y)
			{
				self->rect.pos.y = map->tiles[i].rect.pos.y - self->rect.height;
			}
			else
			{
				self->rect.pos.y = map->tiles[i].rect.pos.y 
					+ map->tiles[i].rect.height;
			}

			self->force.y = 0.0;
			//self->direction.y = 0.0;
		}
	}

	if(self->rect.pos.x < 0)
	{
		self->rect.pos.x = 0;
		self->force.x = 0.0;
	}
	if(self->rect.pos.y < 0)
	{
		self->rect.pos.y = 0;
		self->force.y = 0.0;
	}
	if(self->rect.pos.x + self->rect.width > MAP_WIDTH)
	{
		self->rect.pos.x = MAP_WIDTH - self->rect.width;
		self->force.x = 0.0;
	}
	if(self->rect.pos.y + self->rect.height > MAP_HEIGHT)
	{
		self->rect.pos.y = MAP_HEIGHT - self->rect.height;
		self->force.y = 0.0;
	}

	//Animation stuff
	self->direction = olddirection;
	int prevanimation = self->selectedanimation;
	int stoppedx = 0;
	int stoppedy = 0;

	if(self->direction.y > 0.0)
	{
		self->selectedanimation = PLAYERWALKING_DOWN;
	}
	else if(self->direction.y < 0.0)
	{
		self->selectedanimation = PLAYERWALKING_UP;
	}
	else
	{
		stoppedy = 1;
	}

	if(self->direction.x > 0.0)
	{
		if(fabs(self->direction.y) < 0.7) //Set to 0.71 to make change animation
		{
			self->selectedanimation = PLAYERWALKING_RIGHT;
		}
	}
	else if(self->direction.x < 0.0)
	{
		if(fabs(self->direction.y) < 0.7) //Set to 0.71 to make change animation
		{
			self->selectedanimation = PLAYERWALKING_LEFT;
		}
	}
	else
	{
		stoppedx = 1;
	}

	if(stoppedx && (prevanimation == PLAYERWALKING_LEFT 
		|| prevanimation == PLAYERWALKING_RIGHT))
	{
		self->selectedanimation = prevanimation - PLAYERSTANDING_LEFT - 1;
	}
	else if(stoppedy && (prevanimation == PLAYERWALKING_UP 
		|| prevanimation == PLAYERWALKING_DOWN))
	{
		self->selectedanimation = prevanimation - PLAYERSTANDING_LEFT - 1;
	}

	self->direction.x = 0.0;
	self->direction.y = 0.0;
	if(self->force.x < 0.0)
	{
		self->force.x += PLAYER_DASHDECAY;
	}
	else if(self->force.x > 0.0)
	{
		self->force.x -= PLAYER_DASHDECAY;
	}
	if(self->force.y < 0.0)
	{
		self->force.y += PLAYER_DASHDECAY;
	}
	else if(self->force.y > 0.0)
	{
		self->force.y -= PLAYER_DASHDECAY;
	}
}

void player_render(
	struct Player* self, 
	double interpolation, 
	double delta, 
	double camerax, 
	double cameray)
{ 
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

