#ifndef PLAYER_H
#define PLAYER_H

#include "API2/rect.h"

struct Player
{
	struct Rect rect;
	struct Vec2d oldpos;
	struct Vec2d velocity;
	int hp;
};

#endif
