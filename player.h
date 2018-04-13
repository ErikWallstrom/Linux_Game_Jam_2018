#ifndef PLAYER_H
#define PLAYER_H

#include "API2/transition.h"
#include "API2/animation.h"
#include "API2/texture.h"
#include "API2/rect.h"
#include "map.h"

#define PLAYER_SPEED     400.0
#define PLAYER_WIDTH     10.0
#define PLAYER_HEIGHT    16.0
#define PLAYER_SCALE     4.0
#define PLAYER_DASHDECAY 3

enum PlayerStanding
{
	PLAYERSTANDING_FRONT,
	PLAYERSTANDING_BACK,
	PLAYERSTANDING_RIGHT,
	PLAYERSTANDING_LEFT
};

enum PlayerWalking
{
	PLAYERWALKING_DOWN = PLAYERSTANDING_LEFT + 1,
	PLAYERWALKING_UP,
	PLAYERWALKING_RIGHT,
	PLAYERWALKING_LEFT,
	NUM_ANIMATIONS
};

struct Player
{
	struct Animation animations[NUM_ANIMATIONS];
	struct Transition invincibility;
	struct Texture spritesheet;
	struct Rect rect;
	struct Vec2d oldpos;
	struct Vec2d direction;
	struct Vec2d force;
	SDL_Renderer* renderer;
	size_t selectedanimation;
	int oldhp;
	int hp;
};

struct Player* player_ctor(
	struct Player* self, 
	struct Vec2d pos, 
	SDL_Renderer* renderer);
void player_update(struct Player* self, struct Map* map);
void player_render(
	struct Player* self, 
	double interpolation, 
	double delta, 
	double camerax, 
	double cameray);
void player_dtor(struct Player* self);

#endif
