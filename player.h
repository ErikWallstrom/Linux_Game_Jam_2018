#ifndef PLAYER_H
#define PLAYER_H

#include "API2/animation.h"
#include "API2/texture.h"
#include "API2/window.h"
#include "API2/rect.h"

#define PLAYER_SPEED (400.0 / TICKS_PER_SEC) 
#define PLAYER_WIDTH 10.0
#define PLAYER_HEIGHT 16.0
#define PLAYER_SCALE 5.0

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
	struct Texture spritesheet;
	struct Rect rect;
	struct Vec2d oldpos;
	struct Vec2d velocity;
	struct Vec2d force;
	SDL_Renderer* renderer;
	size_t selectedanimation;
	int hp;
};

struct Player* player_ctor(struct Player* self, SDL_Renderer* renderer);
void player_update(struct Player* self);
void player_render(
	struct Player* self, 
	double interpolation, 
	double delta, 
	double camerax, 
	double cameray);
void player_dtor(struct Player* self);

#endif
