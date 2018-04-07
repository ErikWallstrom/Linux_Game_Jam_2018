#ifndef TRANSITION_H
#define TRANSITION_H

enum TransitionType
{
	TRANSITIONTYPE_DEFAULT,
	TRANSITIONTYPE_REPEAT,
	TRANSITIONTYPE_FADE
};

struct Transition
{
	double from, to;
	double speed;
	double* value;
	int done;
	enum TransitionType type;
};

struct Transition* transition_ctor(
	struct Transition* self, 
	enum TransitionType type,
	double from, 
	double to, 
	double duration,
	double* value
);

void transition_update(struct Transition* self, double delta);

#endif
