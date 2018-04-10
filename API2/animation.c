#include "animation.h"
#include "log.h"

void animation_update(struct Animation* self, double delta)
{
	log_assert(self, "is NULL");

	self->time += delta;
	if(self->time - self->oldtime >= self->delay)
	{
		self->oldtime = self->time;
		self->frame++;

		if(vec_getsize(self->srects) <= self->frame)
		{
			self->frame = 0;
		}
	}
}

