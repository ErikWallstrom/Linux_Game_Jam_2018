#ifndef RECT_H
#define RECT_H

#include "vec2d.h"

enum RectRegPoint
{
	RECTREGPOINT_CENTER,
	RECTREGPOINT_CENTERTOP,
	RECTREGPOINT_CENTERLEFT,
	RECTREGPOINT_CENTERRIGHT,
	RECTREGPOINT_CENTERBOTTOM,

	RECTREGPOINT_TOPLEFT,
	RECTREGPOINT_TOPRIGHT,
	RECTREGPOINT_BOTTOMLEFT,
	RECTREGPOINT_BOTTOMRIGHT
};

enum RectCollisionSide
{
	RECTCOLLISIONSIDE_NONE,
	RECTCOLLISIONSIDE_TOP,
	RECTCOLLISIONSIDE_BOTTOM,
	RECTCOLLISIONSIDE_LEFT,
	RECTCOLLISIONSIDE_RIGHT,
};

//Should it really be called Rect?
struct Rect
{
	struct Vec2d pos;
	double width, height;
};

struct Rect* rect_ctor(
	struct Rect* self, 
	struct Vec2d pos, 
	enum RectRegPoint rpoint, 
	double width, 
	double height
);

struct Vec2d rect_getpos(struct Rect* self, enum RectRegPoint rpoint);
void rect_setpos(
	struct Rect* self, 
	struct Vec2d pos, 
	enum RectRegPoint rpoint
);

int rect_intersects(struct Rect* self, struct Rect* other);
int rect_intersectspoint(struct Rect* self, double x, double y);
enum RectCollisionSide rect_hitside(
	struct Rect* self, 
	struct Rect* other, 
	struct Vec2d speed
);

#endif
