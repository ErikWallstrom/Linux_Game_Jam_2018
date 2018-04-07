#include "str.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct Str* str_ctor(struct Str* self, const char* str)
{ 
	log_assert(self, "is NULL");
	log_assert(str, "is NULL");

	size_t len = strlen(str);
	self->data = malloc(len + 1);
	if(!self->data)
	{ 
		log_error("malloc failed in <%s>", __func__);
	}

	memcpy(self->data, str, len + 1);
	self->len = len;
	return self;
}

struct Str* str_ctorfmt(struct Str* self, const char* fmt, ...) 
{ 
	log_assert(fmt, "is NULL");

	va_list vlist1, vlist2;
	va_start(vlist1, fmt);
	va_copy(vlist2, vlist1);

	size_t len = vsnprintf(NULL, 0, fmt, vlist1);
	self->data = malloc(len + 1);
	if(!self->data)
	{ 
		log_error("malloc failed in <%s>", __func__);
	}

	vsprintf(self->data, fmt, vlist2);
	self->len = len;

	va_end(vlist2);
	va_end(vlist1);
	return self;
}

void str_insert(struct Str* self, size_t index, const char* str)
{ 
	log_assert(self, "is NULL");
	log_assert(index <= self->len, "is NULL");
	log_assert(str, "is NULL");

	size_t len = strlen(str);
	self->data = realloc(self->data, self->len + len + 1);
	if(!self->data)
	{ 
		log_error("realloc failed in <%s>", __func__);
	}

	memmove(
		self->data + index + len, 
		self->data + index, 
		self->len - index + 1
	);

	memcpy(self->data + index, str, len);
	self->len += len;
}

static void str_insertfmtva(
	struct Str* self, 
	size_t index, 
	const char* fmt, 
	va_list vlist)
{ 
	va_list vlist2;
	va_copy(vlist2, vlist);

	size_t len = vsnprintf(NULL, 0, fmt, vlist);
	self->data = realloc(self->data, self->len + len + 1);
	if(!self->data)
	{ 
		log_error("realloc failed in <%s>", __func__);
	}

	memmove(
		self->data + index + len, 
		self->data + index, 
		self->len - index + 1
	);

	char* buffer = malloc(len + 1);
	if(!buffer)
	{ 
		log_error("malloc failed in <%s>", __func__);
	}

	vsprintf(buffer, fmt, vlist2);
	memcpy(self->data + index, buffer, len);
	free(buffer);
	self->len += len;

	va_end(vlist2);
}

void str_insertfmt(struct Str* self, size_t index, const char* fmt, ...)
{ 
	log_assert(self, "is NULL");
	log_assert(index <= self->len, "is NULL");
	log_assert(fmt, "is NULL");

	va_list vlist;
	va_start(vlist, fmt);
	str_insertfmtva(self, index, fmt, vlist);
	va_end(vlist);
}

void str_append(struct Str* self, const char* str)
{ 
	log_assert(self, "is NULL");
	log_assert(str, "is NULL");

	str_insert(self, self->len, str);
}

void str_appendfmt(struct Str* self, const char* fmt, ...)
{ 
	log_assert(self, "is NULL");
	log_assert(fmt, "is NULL");

	va_list vlist;
	va_start(vlist, fmt);
	str_insertfmtva(self, self->len, fmt, vlist);
	va_end(vlist);
}

void str_prepend(struct Str* self, const char* str)
{ 
	log_assert(self, "is NULL");
	log_assert(str, "is NULL");

	str_insert(self, 0, str);
}

void str_prependfmt(struct Str* self, const char* fmt, ...)
{ 
	log_assert(self, "is NULL");
	log_assert(fmt, "is NULL");

	va_list vlist;
	va_start(vlist, fmt);
	str_insertfmtva(self, 0, fmt, vlist);
	va_end(vlist);
}

void str_dtor(struct Str* self)
{ 
	log_assert(self, "is NULL");
	free(self->data);
}

