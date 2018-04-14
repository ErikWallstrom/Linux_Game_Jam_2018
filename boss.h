#ifndef BOSS_H
#define BOSS_H

#include "API2/transition.h"
#include "API2/texture.h"
#include "API2/rect.h"
#include "projectile.h"
#include "player.h"

#define BOSS_SPEED     300.0
#define BOSS_WIDTH     32.0
#define BOSS_HEIGHT    32.0
#define BOSS_SCALE     6.0
#define BOSS_DASHDECAY 2

struct Boss
{
	struct Projectiles projectiles;
	struct Transition dashtimer;
	struct Transition shoottimer;
	struct Transition rotation;
	struct Texture texture;
	struct Rect rect;
	struct Vec2d oldpos;
	struct Vec2d direction;
	struct Vec2d force;
	SDL_Renderer* renderer;
	int hp;
};

struct Boss* boss_ctor(struct Boss* self, SDL_Renderer* renderer);
void boss_update(
	struct Boss* self, 
	struct Player* player,
	struct Projectiles* projectiles,
	struct Map* map
);
void boss_render(
	struct Boss* self, 
	double interpolation, 
	double delta, 
	double camerax, 
	double cameray
);
void boss_dtor(struct Boss* self);

#endif
