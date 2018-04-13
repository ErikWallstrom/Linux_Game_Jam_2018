#ifndef ENEMY_H
#define ENEMY_H

#include "API2/texture.h"
#include "API2/rect.h"
#include "projectile.h"
#include "player.h"
#include "map.h"

#define ENEMY_WIDTH     16.0
#define ENEMY_HEIGHT    16.0
#define ENEMY_SCALE     4.0

enum EnemyType
{
	ENEMYTYPE_GHOST,
	ENEMYTYPE_SLIME,
	ENEMYTYPE_STONE,
	ENEMYTYPE_TREE,
};

struct Enemy
{
	struct Rect rect;
	struct Vec2d oldpos;
	enum EnemyType type;
	int hp;
};

struct Enemies
{
	struct Texture enemysheet;
	Vec(struct Enemy) enemies;
	SDL_Renderer* renderer;
};

struct Enemies* enemies_ctor(struct Enemies* self, SDL_Renderer* renderer);
void enemies_ghosts(struct Enemies* self, size_t num);
void enemies_slimes(struct Enemies* self, size_t num);
void enemies_terrain(struct Enemies* self, struct Map* map);
void enemies_update(
	struct Enemies* self, 
	struct Map* map, 
	struct Player* player,
	struct Projectiles* projectiles
);
void enemies_render(
	struct Enemies* self, 
	double interpolation,
	double camerax, 
	double cameray
);
void enemies_dtor(struct Enemies* self);

#endif
