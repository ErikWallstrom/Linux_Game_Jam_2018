#include "transition.h"
#include "log.h"

struct Transition* transition_ctor(
	struct Transition* self, 
	enum TransitionType type,
	double from, 
	double to, 
	double duration,
	double* value
)
{
	log_assert(self, "is NULL");
	log_assert(type <= TRANSITIONTYPE_FADE, "invalid type: %i", type);
	log_assert(duration, "is zero");
	log_assert(value, "is NULL");

	self->from = from;
	self->to = to;
	self->done = 0;
	self->type = type;
	self->value = value;
	self->speed = (to - from) / (duration / 1000.0);
	*value = from;

	return self;
}

void transition_update(struct Transition* self, double delta)
{
	log_assert(self, "is NULL");
	if(!self->done)
	{
		*self->value += self->speed * (delta / 1000.0);
		
		if(*self->value > self->to)
		{
			*self->value = self->to;
			switch(self->type)
			{
			case TRANSITIONTYPE_DEFAULT:
				self->done = 1;
				break;
			case TRANSITIONTYPE_REPEAT:
				*self->value = self->from;
				break;
			case TRANSITIONTYPE_FADE:
				self->speed = -self->speed;
				break;
			}
		}
		else if(*self->value < self->from)
		{
			*self->value = self->from;
			switch(self->type)
			{
			case TRANSITIONTYPE_DEFAULT:
				//self->done = 1; Nothing should happen right?
				break;
			case TRANSITIONTYPE_REPEAT:
				*self->value = self->to;
				break;
			case TRANSITIONTYPE_FADE:
				self->speed = -self->speed;
				break;
			}
		}
	}
}

