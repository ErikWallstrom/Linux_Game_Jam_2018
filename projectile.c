#include "projectile.h"
#include "API2/log.h"

extern const double TICK_RATE; //XXX: hacky

struct Projectiles* projectiles_ctor(
	struct Projectiles* self, 
	SDL_Renderer* renderer)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	self->projectiles = vec_ctor(struct Projectile, 100);
	self->renderer = renderer;
	texture_ctorimage(
		&self->projectilesheet, 
		"resources/projectiles.png",
		self->renderer
	);

	return self;
}

void projectiles_add(
	struct Projectiles* self, 
	struct Vec2d start, 
	double rotation, 
	enum ProjectileType type)
{
	log_assert(self, "is NULL");
	
	struct Projectile projectile = {
		.rotation = rotation,
		.start = start, 
		.type = type,
		.done = 0
	};

	rect_ctor(
		&projectile.rect, 
		start, 
		RECTREGPOINT_CENTER, 
		PROJECTILE_WIDTH * PROJECTILE_SCALE,
		PROJECTILE_HEIGHT * PROJECTILE_SCALE
	);

	vec_pushback(self->projectiles, projectile);
}

void projectiles_update(struct Projectiles* self, struct Map* map)
{
	log_assert(self, "is NULL");
	log_assert(map, "is NULL");
	
	for(size_t i = 0; i < vec_getsize(self->projectiles); i++)
	{
		if(!self->projectiles[i].done)
		{
			self->projectiles[i].oldpos = self->projectiles[i].rect.pos;
			self->projectiles[i].rect.pos.x 
				+= cos(self->projectiles[i].rotation)
				* (PROJECTILE_SPEED / TICK_RATE);
			self->projectiles[i].rect.pos.y 
				+= sin(self->projectiles[i].rotation)
				* (PROJECTILE_SPEED / TICK_RATE);
			
			for(size_t j = 0; j < vec_getsize(map->tiles); j++)
			{
				if(rect_intersects(
					&self->projectiles[i].rect, 
					&map->tiles[j].rect))
				{
					self->projectiles[i].done = 1;
					break;
				}
			}
				
			struct Vec2d temp;
			vec2d_sub(
				&self->projectiles[i].rect.pos, 
				&self->projectiles[i].start,
				&temp
			);

			double length;
			vec2d_length(&temp, &length);
			int isspecial = self->projectiles[i].type;
			if(length > ((isspecial) 
				? PROJECTILE_RANGE * 1.3 
				: PROJECTILE_RANGE))
			{
				self->projectiles[i].done = 1;
			}
		}
	}

	//TODO: Clean done projectiles (memory leak)
}

void projectiles_render(
	struct Projectiles* self, 
	double interpolation, 
	double camerax, 
	double cameray)
{
	log_assert(self, "is NULL");
	
	for(size_t i = 0; i < vec_getsize(self->projectiles); i++)
	{
		if(!self->projectiles[i].done)
		{
			SDL_RenderCopyEx(
				self->renderer, 
				self->projectilesheet.raw, 
				&(SDL_Rect){
					self->projectiles[i].type * PROJECTILE_WIDTH,
					0,
					PROJECTILE_WIDTH,
					PROJECTILE_HEIGHT
				},
				&(SDL_Rect){
					self->projectiles[i].oldpos.x
						+ (self->projectiles[i].rect.pos.x
						- self->projectiles[i].oldpos.x) * interpolation
						- camerax,
					self->projectiles[i].oldpos.y
						+ (self->projectiles[i].rect.pos.y 
						- self->projectiles[i].oldpos.y) * interpolation
						- cameray,
					self->projectiles[i].rect.width,
					self->projectiles[i].rect.height
				},
				self->projectiles[i].rotation / (M_PI / 180.0),
				NULL,
				SDL_FLIP_NONE
			);
		}
	}
}

void projectiles_dtor(struct Projectiles* self)
{
	log_assert(self, "is NULL");
	vec_dtor(self->projectiles);
	texture_dtor(&self->projectilesheet);
}

