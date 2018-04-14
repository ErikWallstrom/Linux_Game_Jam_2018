#include "boss.h"
#include "map.h"
#include "API2/log.h"

#define BOSS_HP 5000
#define BOSS_DMG 10
extern const double TICK_RATE; //XXX: hacky

struct Boss* boss_ctor(struct Boss* self, SDL_Renderer* renderer)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	self->rect.width = BOSS_WIDTH * BOSS_SCALE;
	self->rect.height = BOSS_HEIGHT * BOSS_SCALE;
	self->rect.pos.x = rand() % (int)(MAP_WIDTH - self->rect.width);
	self->rect.pos.y = rand() % (int)(MAP_HEIGHT - self->rect.height);
	self->oldpos = self->rect.pos;
	self->hp = BOSS_HP;
	self->renderer = renderer;
	self->force = (struct Vec2d){0};

	texture_ctorimage(&self->texture, "resources/boss.png", renderer);
	static double rotation;
	transition_ctor(
		&self->rotation, 
		TRANSITIONTYPE_REPEAT, 
		0.0, 
		360.0, 
		500.0,  
		&rotation
	);
	static double shoottimer;
	transition_ctor(
		&self->shoottimer, 
		TRANSITIONTYPE_DEFAULT, 
		0.0, 
		100.0, 
		100.0,  
		&shoottimer
	);

	static double dashtimer;
	transition_ctor(
		&self->dashtimer, 
		TRANSITIONTYPE_DEFAULT, 
		0.0, 
		2200.0, 
		2200.0,  
		&dashtimer
	);

	projectiles_ctor(&self->projectiles, renderer);
	return self;
}

void boss_update(
	struct Boss* self, 
	struct Player* player,
	struct Projectiles* projectiles,
	struct Map* map)
{
	log_assert(self, "is NULL");

	struct Vec2d direction;
	vec2d_sub(&player->rect.pos, &self->rect.pos, &direction);
	vec2d_normalize(&direction, &direction);

	self->oldpos = self->rect.pos;
	if(self->hp > 4000)
	{
		self->rect.pos.x += direction.x * (BOSS_SPEED / TICK_RATE);
		self->rect.pos.y += direction.y * (BOSS_SPEED / TICK_RATE);
	}
	else if(self->hp > 1500)
	{
		self->rect.pos.x += direction.x * ((BOSS_SPEED * 0.3) / TICK_RATE);
		self->rect.pos.y += direction.y * ((BOSS_SPEED * 0.3) / TICK_RATE);
		
		transition_update(&self->shoottimer, TICK_RATE);
		if(self->shoottimer.done)
		{
			self->shoottimer.done = 0;
			*self->shoottimer.value = 0.0;

			struct Vec2d pos = rect_getpos(&self->rect, RECTREGPOINT_CENTER);
			projectiles_add(
				&self->projectiles, 
				pos, 
				*self->rotation.value,
				PROJECTILETYPE_NORMAL
			);
		}

		projectiles_updateboss(&self->projectiles, map, player);
	}
	else
	{
		self->rect.pos.x += direction.x * ((BOSS_SPEED * 0.2) / TICK_RATE);
		self->rect.pos.y += direction.y * ((BOSS_SPEED * 0.2) / TICK_RATE);

		transition_update(&self->shoottimer, TICK_RATE);
		if(self->shoottimer.done)
		{
			self->shoottimer.done = 0;
			*self->shoottimer.value = 0.0;

			struct Vec2d pos = rect_getpos(&self->rect, RECTREGPOINT_CENTER);
			projectiles_add(
				&self->projectiles, 
				pos, 
				*self->rotation.value,
				PROJECTILETYPE_NORMAL
			);
		}

		projectiles_updateboss(&self->projectiles, map, player);
		SDL_SetTextureColorMod(self->texture.raw, 255, 0, 0);
		transition_update(&self->dashtimer, TICK_RATE);
		if(self->dashtimer.done)
		{
			self->dashtimer.done = 0;
			*self->dashtimer.value = 0.0;

			struct Vec2d knockback;
			vec2d_sub(
				(struct Vec2d[1]){
					rect_getpos(&player->rect, RECTREGPOINT_CENTER)
				},
				(struct Vec2d[1]){
					rect_getpos(&self->rect, RECTREGPOINT_CENTER)
				},
				&knockback
			);

			if(fabs(knockback.x) > fabs(knockback.y))
			{
				if(player->invincibility.done)
				{
					if(knockback.x < 0.0)
					{
						self->force.x = -BOSS_DASHDECAY * 22;
					}
					else
					{
						self->force.x = BOSS_DASHDECAY * 22;
					}
				}
			}
			else
			{
				if(player->invincibility.done)
				{
					if(knockback.y < 0.0)
					{
						self->force.y = -BOSS_DASHDECAY * 22;
					}
					else
					{
						self->force.y = BOSS_DASHDECAY * 22;
					}
				}
			}
		}

		self->rect.pos.x += self->force.x;
		self->rect.pos.y += self->force.y;

		if(self->force.x < 0.0)
		{
			self->force.x += BOSS_DASHDECAY;
		}
		else if(self->force.x > 0.0)
		{
			self->force.x -= BOSS_DASHDECAY;
		}
		if(self->force.y < 0.0)
		{
			self->force.y += BOSS_DASHDECAY;
		}
		else if(self->force.y > 0.0)
		{
			self->force.y -= BOSS_DASHDECAY;
		}
	}

	if(rect_intersects(&self->rect, &player->rect))
	{
		player->hp -= BOSS_DMG;
		struct Vec2d knockback;
		vec2d_sub(
			(struct Vec2d[1]){
				rect_getpos(&player->rect, RECTREGPOINT_CENTER)
			},
			(struct Vec2d[1]){
				rect_getpos(&self->rect, RECTREGPOINT_CENTER)
			},
			&knockback
		);

		if(fabs(knockback.x) > fabs(knockback.y))
		{
			if(player->invincibility.done)
			{
				if(knockback.x < 0.0)
				{
					player->force.x = -PLAYER_DASHDECAY * 10;
				}
				else
				{
					player->force.x = PLAYER_DASHDECAY * 10;
				}
			}
		}
		else
		{
			if(player->invincibility.done)
			{
				if(knockback.y < 0.0)
				{
					player->force.y = -PLAYER_DASHDECAY * 10;
				}
				else
				{
					player->force.y = PLAYER_DASHDECAY * 10;
				}
			}
		}
	}

	for(size_t j = 0; j < vec_getsize(projectiles->projectiles); j++)
	{
		if(!projectiles->projectiles[j].done
			&& rect_intersects(&self->rect, &projectiles->projectiles[j].rect))
		{
			projectiles->projectiles[j].done = 1;
			self->hp -= (projectiles->projectiles[j].type
				== PROJECTILETYPE_NORMAL) 
				? NORMAL_DMG 
				: SPECIAL_DMG;
		}
	}
}

void boss_render(
	struct Boss* self, 
	double interpolation, 
	double delta, 
	double camerax, 
	double cameray)
{
	log_assert(self, "is NULL");
	if(self->hp <= 4000)
	{
		transition_update(&self->rotation, delta);
	}
	
	projectiles_render(&self->projectiles, interpolation, camerax, cameray);
	if(self->hp > 0)
	{
		SDL_RenderCopyEx(
			self->renderer, 
			self->texture.raw, 
			NULL,
			&(SDL_Rect){
				self->oldpos.x + (self->rect.pos.x - self->oldpos.x)
					* interpolation - camerax,
				self->oldpos.y + (self->rect.pos.y - self->oldpos.y)
					* interpolation - cameray,
				self->rect.width,
				self->rect.height
			},
			*self->rotation.value,
			NULL,
			SDL_FLIP_NONE
		);
	}
}

void boss_dtor(struct Boss* self)
{
	log_assert(self, "is NULL");
	
}

