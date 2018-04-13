#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "API2/texture.h"
#include "API2/rect.h"
#include "API2/vec.h"
#include "map.h"

#define PROJECTILE_RANGE  420.0
#define PROJECTILE_SPEED  800.0
#define PROJECTILE_WIDTH  8.0
#define PROJECTILE_HEIGHT 8.0
#define PROJECTILE_SCALE  4.0
#define SPECIAL_DMG 100
#define NORMAL_DMG 50

enum ProjectileType
{
	PROJECTILETYPE_NORMAL,
	PROJECTILETYPE_SPECIAL
};

struct Projectile
{
	struct Rect rect;
	struct Vec2d oldpos;
	struct Vec2d start;
	double rotation;
	enum ProjectileType type;
	int done;
};

struct Projectiles
{
	struct Texture projectilesheet;
	Vec(struct Projectile) projectiles;
	SDL_Renderer* renderer;
};

struct Projectiles* projectiles_ctor(
	struct Projectiles* self, 
	SDL_Renderer* renderer
);
void projectiles_add(
	struct Projectiles* self, 
	struct Vec2d start, 
	double rotation, 
	enum ProjectileType type
);
void projectiles_update(struct Projectiles* self, struct Map* map);
void projectiles_render(
	struct Projectiles* self, 
	double interpolation,
	double camerax, 
	double cameray
);
void projectiles_dtor(struct Projectiles* self);

#endif
