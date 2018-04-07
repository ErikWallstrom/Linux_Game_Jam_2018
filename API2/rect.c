#include "rect.h"
#include "log.h"

struct Rect* rect_ctor(
	struct Rect* self, 
	struct Vec2d pos, 
	enum RectRegPoint rpoint, 
	double width, 
	double height
)
{
	log_assert(self, "is NULL");
	log_assert(
		rpoint <= RECTREGPOINT_BOTTOMRIGHT, 
		"invalid rpoint (%i)", 
		rpoint
	);

	self->width = width;
	self->height = height;
	rect_setpos(self, pos, rpoint);

	return self;
}

struct Vec2d rect_getpos(struct Rect* self, enum RectRegPoint rpoint)
{
	log_assert(self, "is NULL");
	log_assert(
		rpoint <= RECTREGPOINT_BOTTOMRIGHT, 
		"invalid rpoint (%i)", 
		rpoint
	);

	struct Vec2d pos;
	switch(rpoint)
	{
	case RECTREGPOINT_CENTER:
		pos.x = self->pos.x + self->width / 2.0;
		pos.y = self->pos.y + self->height / 2.0;
		break;
	case RECTREGPOINT_CENTERTOP:
		pos.x = self->pos.x + self->width / 2.0;
		pos.y = self->pos.y;
		break;
	case RECTREGPOINT_CENTERLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y + self->height / 2.0;
		break;
	case RECTREGPOINT_CENTERRIGHT:
		pos.x = self->pos.x + self->width;
		pos.y = self->pos.y + self->height / 2.0;
		break;
	case RECTREGPOINT_CENTERBOTTOM:
		pos.x = self->pos.x + self->width / 2.0;
		pos.y = self->pos.y + self->height;
		break;
	case RECTREGPOINT_TOPLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y;
		break;
	case RECTREGPOINT_TOPRIGHT:
		pos.x = self->pos.x + self->width;
		pos.y = self->pos.y;
		break;
	case RECTREGPOINT_BOTTOMLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y + self->height;
		break;
	case RECTREGPOINT_BOTTOMRIGHT:
		pos.x = self->pos.x + self->width;
		pos.y = self->pos.y + self->height;
		break;
	}

	return pos;
}

void rect_setpos(
	struct Rect* self, 
	struct Vec2d pos, 
	enum RectRegPoint rpoint
)
{
	log_assert(self, "is NULL");
	log_assert(
		rpoint <= RECTREGPOINT_BOTTOMRIGHT, 
		"invalid rpoint (%i)", 
		rpoint
	);

	switch(rpoint)
	{
	case RECTREGPOINT_CENTER:
		self->pos.x = pos.x - self->width / 2.0;
		self->pos.y = pos.y - self->height / 2.0;
		break;
	case RECTREGPOINT_CENTERTOP:
		self->pos.x = pos.x - self->width / 2.0;
		self->pos.y = pos.y;
		break;
	case RECTREGPOINT_CENTERLEFT:
		self->pos.x = pos.x;
		self->pos.y = pos.y - self->height / 2.0;
		break;
	case RECTREGPOINT_CENTERRIGHT:
		self->pos.x = pos.x - self->width;
		self->pos.y = pos.y - self->height / 2.0;
		break;
	case RECTREGPOINT_CENTERBOTTOM:
		self->pos.x = pos.x - self->width / 2.0;
		self->pos.y = pos.y - self->height;
		break;
	case RECTREGPOINT_TOPLEFT:
		self->pos.x = pos.x;
		self->pos.y = pos.y;
		break;
	case RECTREGPOINT_TOPRIGHT:
		self->pos.x = pos.x - self->width;
		self->pos.y = pos.y;
		break;
	case RECTREGPOINT_BOTTOMLEFT:
		self->pos.x = pos.x;
		self->pos.y = pos.y - self->height;
		break;
	case RECTREGPOINT_BOTTOMRIGHT:
		self->pos.x = pos.x - self->width;
		self->pos.y = pos.y - self->height;
		break;
	}
}

static int intersectsaxis(
	double amin, 
	double amax, 
	double bmin, 
	double bmax
)
{
	if(bmin > amin)
	{
		amin = bmin;
	}

	if(bmax < amax)
	{
		amax = bmax;
	}

	if(amax <= amin)
	{
		return 0;
	}

	return 1;
}

int rect_intersects(struct Rect* self, struct Rect* other)
{
	log_assert(self, "is NULL");
	log_assert(other, "is NULL");

	struct Vec2d pos1 = self->pos;
	struct Vec2d pos2 = other->pos;

	int intersectx = intersectsaxis(
		pos1.x, 
		pos1.x + self->width, 
		pos2.x, 
		pos2.x + other->width
	);

	if(!intersectx)
	{
		return 0;
	}

	int intersecty = intersectsaxis(
		pos1.y,
		pos1.y + self->height, 
		pos2.y, 
		pos2.y + other->height
	);

	if(!intersecty)
	{
		return 0;
	}

	return 1;
}

int rect_intersectspoint(struct Rect* self, double x, double y)
{
	log_assert(self, "is NULL");

	struct Vec2d pos = self->pos;

	if((x >= pos.x) && (x <= (pos.x + self->width - 1)) && 
		(y >= pos.y) && (y <= (pos.y + self->height - 1)))
	{
		return 1;
	}

	return 0;
}

static int rect_hitleft(struct Rect* self, double dx, struct Rect* other)
{
	log_assert(self, "is NULL");
	log_assert(other, "is NULL");

	struct Vec2d selfpos = self->pos;
	struct Vec2d otherpos = other->pos;

	if(selfpos.x - dx < otherpos.x + other->width)
	{
		return 0;
	}

	if(selfpos.x >= otherpos.x + other->width)
	{
		return 0;
	}

	int intersecty = intersectsaxis(
		selfpos.y,
		selfpos.y + self->height,
		otherpos.y,
		otherpos.y + other->height
	);

	if(!intersecty)
	{
		return 0;
	}

	return 1;
}

static int rect_hitright(struct Rect* self, double dx, struct Rect* other)
{
	log_assert(self, "is NULL");
	log_assert(other, "is NULL");

	struct Vec2d selfpos = self->pos;
	struct Vec2d otherpos = other->pos;

	if(selfpos.x + self->width - dx > otherpos.x)
	{
		return 0;
	}

	if(selfpos.x + self->width <= otherpos.x)
	{
		return 0;
	}

	int intersecty = intersectsaxis(
		selfpos.y,
		selfpos.y + self->height,
		otherpos.y,
		otherpos.y + other->height
	);

	if(!intersecty)
	{
		return 0;
	}

	return 1;
}

static int rect_hittop(struct Rect* self, double dy, struct Rect* other)
{
	log_assert(self, "is NULL");
	log_assert(other, "is NULL");

	struct Vec2d selfpos = self->pos;
	struct Vec2d otherpos = other->pos;

	if(selfpos.y - dy < otherpos.y + other->height)
	{
		return 0;
	}

	if(selfpos.y >= otherpos.y + other->height)
	{
		return 0;
	}

	int intersectx = intersectsaxis(
		selfpos.x,
		selfpos.x + self->width,
		otherpos.x,
		otherpos.x + other->width
	);

	if(!intersectx)
	{
		return 0;
	}

	return 1;

}

static int rect_hitbottom(struct Rect* self, double dy, struct Rect* other)
{
	log_assert(self, "is NULL");
	log_assert(other, "is NULL");

	struct Vec2d selfpos = self->pos;
	struct Vec2d otherpos = other->pos;

	if(selfpos.y + self->height - dy > otherpos.y)
	{
		return 0;
	}

	if(selfpos.y + self->height <= otherpos.y)
	{
		return 0;
	}

	int intersectx = intersectsaxis(
		selfpos.x,
		selfpos.x + self->width,
		otherpos.x,
		otherpos.x + other->width
	);

	if(!intersectx)
	{
		return 0;
	}

	return 1;
}

enum RectCollisionSide rect_hitside(
	struct Rect* self, 
	struct Rect* other, 
	struct Vec2d speed
)
{
	log_assert(self, "is NULL");
	log_assert(other, "is NULL");

	if(rect_hitleft(self, speed.x - 1, other))
	{
		return RECTCOLLISIONSIDE_LEFT;
	}
	else if(rect_hitright(self, speed.x, other))
	{
		return RECTCOLLISIONSIDE_RIGHT;
	}
	else if(rect_hittop(self, speed.y, other))
	{
		return RECTCOLLISIONSIDE_TOP;
	}
	else if(rect_hitbottom(self, speed.y, other))
	{
		return RECTCOLLISIONSIDE_BOTTOM;
	}
	else
	{
		return RECTCOLLISIONSIDE_NONE;
	}
}

