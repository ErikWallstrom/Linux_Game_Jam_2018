#ifndef ANIMATION_H
#define ANIMATION_H

#include "vec.h"

struct Animation
{
	Vec(struct Rect) srects;
	double time, oldtime;
	double delay;
	size_t frame;
};

void animation_update(struct Animation* self, double delta);

#endif
