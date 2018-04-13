#include "enemy.h"
#include "API2/log.h"

#define GHOST_HP    100
#define GHOST_DMG   10
#define GHOST_SPEED 50.0
#define SLIME_HP    100
#define SLIME_DMG   20
#define SLIME_SPEED 150.0
#define TREE_HP     100
#define TREE_DMG    20
#define TREE_SPEED  50.0
#define STONE_HP    200
#define STONE_DMG   20
#define STONE_SPEED 20.0

extern const double TICK_RATE; //XXX: hacky

struct Enemies* enemies_ctor(struct Enemies* self, SDL_Renderer* renderer)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");

	self->enemies = vec_ctor(struct Enemy, 50);
	self->renderer = renderer;
	texture_ctorimage(&self->enemysheet, "resources/enemies.png", renderer);

	return self;
}

void enemies_ghosts(struct Enemies* self, size_t num)
{
	log_assert(self, "is NULL");
	log_assert(num, "is 0");

	for(size_t i = 0; i < num; i++)
	{
		struct Enemy enemy;
		enemy.type = ENEMYTYPE_GHOST;
		enemy.hp = GHOST_HP;
		enemy.rect.width = ENEMY_WIDTH * ENEMY_SCALE;
		enemy.rect.height = ENEMY_HEIGHT * ENEMY_SCALE;
		enemy.rect.pos.x = rand() % (int)(MAP_WIDTH - enemy.rect.width);
		enemy.rect.pos.y = rand() % (int)(MAP_HEIGHT - enemy.rect.height);
		enemy.oldpos = enemy.rect.pos;
		vec_pushback(self->enemies, enemy);
	}
}

void enemies_slimes(struct Enemies* self, size_t num)
{
	log_assert(self, "is NULL");
	log_assert(num, "is 0");

	for(size_t i = 0; i < num; i++)
	{
		struct Enemy enemy;
		enemy.type = ENEMYTYPE_SLIME;
		enemy.hp = SLIME_HP;
		enemy.rect.width = ENEMY_WIDTH * ENEMY_SCALE;
		enemy.rect.height = ENEMY_HEIGHT * ENEMY_SCALE;
		enemy.rect.pos.x = rand() % (int)(MAP_WIDTH - enemy.rect.width);
		enemy.rect.pos.y = rand() % (int)(MAP_HEIGHT - enemy.rect.height);
		enemy.oldpos = enemy.rect.pos;
		vec_pushback(self->enemies, enemy);
	}
}

void enemies_terrain(struct Enemies* self, struct Map* map)
{
	log_assert(self, "is NULL");
	log_assert(map, "is NULL");

	for(size_t i = 0; i < vec_getsize(map->tiles); i++)
	{
		struct Enemy enemy;
		if(map->tiles[i].type == TILETYPE_TREE)
		{
			enemy.type = ENEMYTYPE_TREE;
			enemy.hp = TREE_HP;
		}
		else if(map->tiles[i].type == TILETYPE_STONE)
		{
			enemy.type = ENEMYTYPE_STONE;
			enemy.hp = STONE_HP;
		}

		enemy.rect.pos = map->tiles[i].rect.pos;
		enemy.rect.width = ENEMY_WIDTH * ENEMY_SCALE;
		enemy.rect.height = ENEMY_HEIGHT * ENEMY_SCALE;
		enemy.oldpos = enemy.rect.pos;
		vec_pushback(self->enemies, enemy);
	}

	vec_clear(map->tiles);
}

void enemies_update(
	struct Enemies* self, 
	struct Map* map, 
	struct Player* player,
	struct Projectiles* projectiles)
{
	log_assert(self, "is NULL");
	log_assert(map, "is NULL");
	log_assert(player, "is NULL");
	log_assert(projectiles, "is NULL");
	
	for(size_t i = 0; i < vec_getsize(self->enemies); i++)
	{
		struct Vec2d direction;
		vec2d_sub(&player->rect.pos, &self->enemies[i].rect.pos, &direction);
		double length;
		vec2d_length(&direction, &length);
		int isclose = (length < 400) ? 1 : 0;
		vec2d_normalize(&direction, &direction);

		self->enemies[i].oldpos = self->enemies[i].rect.pos;
		switch(self->enemies[i].type)
		{
		case ENEMYTYPE_TREE:
			self->enemies[i].rect.pos.x += direction.x
				* (TREE_SPEED / TICK_RATE);
			self->enemies[i].rect.pos.y += direction.y 
				* (TREE_SPEED / TICK_RATE);
			break;

		case ENEMYTYPE_STONE:
			self->enemies[i].rect.pos.x += direction.x
				* (STONE_SPEED / TICK_RATE);
			self->enemies[i].rect.pos.y += direction.y 
				* (STONE_SPEED / TICK_RATE);
			break;

		case ENEMYTYPE_GHOST:
			self->enemies[i].rect.pos.x += direction.x
				* (GHOST_SPEED / TICK_RATE);
			self->enemies[i].rect.pos.y += direction.y 
				* (GHOST_SPEED / TICK_RATE);
			break;

		case ENEMYTYPE_SLIME:
			if(isclose)
			{
				self->enemies[i].rect.pos.x += direction.x
					* (SLIME_SPEED / TICK_RATE);
				for(size_t j = 0; j < vec_getsize(map->tiles); j++)
				{
					if(rect_intersects(
						&self->enemies[i].rect, 
						&map->tiles[j].rect))
					{
						struct Vec2d pos1 = rect_getpos(
							&self->enemies[i].rect, 
							RECTREGPOINT_CENTER
						);

						struct Vec2d pos2 = rect_getpos(
							&map->tiles[j].rect, 
							RECTREGPOINT_CENTER
						);

						if(pos1.x < pos2.x)
						{
							self->enemies[i].rect.pos.x
								= map->tiles[j].rect.pos.x 
								- self->enemies[i].rect.width;
						}
						else
						{
							self->enemies[i].rect.pos.x
								= map->tiles[j].rect.pos.x 
								+ map->tiles[j].rect.width;
						}
					}
				}
				self->enemies[i].rect.pos.y += direction.y 
					* (SLIME_SPEED / TICK_RATE);
				for(size_t j = 0; j < vec_getsize(map->tiles); j++)
				{
					if(rect_intersects(
						&self->enemies[i].rect, 
						&map->tiles[j].rect))
					{
						struct Vec2d pos1 = rect_getpos(
							&self->enemies[i].rect, 
							RECTREGPOINT_CENTER
						);

						struct Vec2d pos2 = rect_getpos(
							&map->tiles[j].rect, 
							RECTREGPOINT_CENTER
						);

						if(pos1.y < pos2.y)
						{
							self->enemies[i].rect.pos.y 
								= map->tiles[j].rect.pos.y 
								- self->enemies[i].rect.height;
						}
						else
						{
							self->enemies[i].rect.pos.y
								= map->tiles[j].rect.pos.y 
								+ map->tiles[j].rect.height;
						}
					}
				}
			}
			break;
		}
	}
}

void enemies_render(
	struct Enemies* self, 
	double interpolation,
	double camerax, 
	double cameray)
{
	log_assert(self, "is NULL");
	for(size_t i = 0; i < vec_getsize(self->enemies); i++)
	{
		SDL_RenderCopy(
			self->renderer, 
			self->enemysheet.raw, 
			&(SDL_Rect){
				self->enemies[i].type * ENEMY_WIDTH,
				0,
				ENEMY_WIDTH,
				ENEMY_HEIGHT
			},
			&(SDL_Rect){
				self->enemies[i].oldpos.x
					+ (self->enemies[i].rect.pos.x
					- self->enemies[i].oldpos.x) * interpolation
					- camerax,
				self->enemies[i].oldpos.y
					+ (self->enemies[i].rect.pos.y 
					- self->enemies[i].oldpos.y) * interpolation
					- cameray,
				self->enemies[i].rect.width,
				self->enemies[i].rect.height
			}
		);
	}
}

void enemies_dtor(struct Enemies* self)
{
	log_assert(self, "is NULL");

	vec_dtor(self->enemies);
	texture_dtor(&self->enemysheet);
}

