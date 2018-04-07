#include "vec2d.h"
#include "log.h"
#include <math.h>

struct Vec2d* vec2d_ctor(struct Vec2d* self, double x, double y)
{
	log_assert(self, "is NULL");
	self->x = x;
	self->y = y;

	return self;
}

void vec2d_add(struct Vec2d* self, struct Vec2d* vec, struct Vec2d* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x + vec->x;
	dest->y = self->y + vec->y;
}

void vec2d_sub(struct Vec2d* self, struct Vec2d* vec, struct Vec2d* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x - vec->x;
	dest->y = self->y - vec->y;
}

void vec2d_scale(struct Vec2d* self, double scalar, struct Vec2d* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x * scalar;
	dest->y = self->y * scalar;
}

void vec2d_negate(struct Vec2d* self, struct Vec2d* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = -self->x;
	dest->y = -self->y;
}

void vec2d_length(struct Vec2d* self, double* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	*dest = sqrt(self->x * self->x + self->y * self->y);
}

void vec2d_normalize(struct Vec2d* self, struct Vec2d* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	double length;
	vec2d_length(self, &length);
	if(length == 0.0) //Prevent division by zero exceptions
	{
		dest->x = 0.0;
		dest->y = 0.0;
	}
	else
	{
		dest->x = self->x / length;
		dest->y = self->y / length;
	}
}

