#include "vec.h"
#include "log.h"
#include <stdlib.h>

#define VEC_DEFAULT_SIZE 5
#define VEC_NEW_SIZE 5

#define vec_tovector(v) \
	(struct Vec_*)((*(char**)(v)) - offsetof(struct Vec_, buffer))

struct Vec_
{
	size_t size;
	size_t buffersize;
	size_t elementsize;
	char buffer[];
};

Vec(void) vec_ctor_(size_t elementsize, size_t elements)
{
	log_assert(elementsize, "must be at least 1");

	//If elements is 0, use default size
	size_t buffersize = (elements ? elements : VEC_DEFAULT_SIZE) 
		* elementsize;
	struct Vec_* self = malloc(sizeof(struct Vec_) + 
		buffersize + sizeof(unsigned int));
	if(!self)
	{
		log_error("malloc failed, in <%s>", __func__);
	}

	self->buffersize = buffersize;
	self->elementsize = elementsize;
	self->size = 0;

	return self->buffer;
}

void vec_dtor_(Vec(void) vec)
{
	log_assert(vec, "is NULL");

	struct Vec_* self = vec_tovector(vec);
	free(self);
}

size_t vec_getsize_(Vec(void) vec)
{
	log_assert(vec, "is NULL");

	struct Vec_* self = vec_tovector(vec);
	return self->size;
}

void vec_expand_(Vec(void) vec, size_t pos, size_t elements)
{
	log_assert(vec, "is NULL");
	log_assert(elements, "Expanding vec with 0 is unnecessary");

	struct Vec_* self = vec_tovector(vec);
	log_assert(
		self->size >= pos, 
		"Index is out of bounds (self->size: %zu, pos: %zu)", 
		self->size, 
		pos
	);

	self->size += elements;
	if(self->buffersize < self->size * self->elementsize)
	{
		self->buffersize = self->size * self->elementsize + 
			self->elementsize * VEC_NEW_SIZE;
		self = realloc(
			self, 
			sizeof(struct Vec_) + self->buffersize +
				sizeof(unsigned int)
		);
		if(!self)
		{
			log_error("realloc failed, in <%s>", __func__);
		}

		*(void**)vec = self->buffer;
	}

	memmove(
		self->buffer + (pos + elements) * self->elementsize, 
		self->buffer + pos * self->elementsize, 
		(self->size - elements - pos) * self->elementsize
	);
}

void vec_collapse_(Vec(void) vec, size_t pos, size_t elements)
{
	log_assert(vec, "is NULL");

	//XXX: Commenting line below makes vec_clear work
	//log_assert(elements, "Collapsing vec with 0 is unnecessary");
	struct Vec_* self = vec_tovector(vec);
	log_assert(
		self->size >= pos, 
		"Index is out of bounds (self->size: %zu, pos: %zu)",
		self->size,
		pos
	);

	memmove(
		self->buffer + pos * self->elementsize, 
		self->buffer + (pos + elements) * self->elementsize,
		(self->size - elements - pos) * self->elementsize
	);
	self->size -= elements;
}

