#ifndef STR_H
#define STR_H

#include <stddef.h>

struct Str
{ 
	size_t len;
	char* data;
};

struct Str* str_ctor(struct Str* self, const char* str);
struct Str* str_ctorfmt(struct Str* self, const char* fmt, ...) 
	__attribute__((format (printf, 2, 3)));
void str_insert(struct Str* self, size_t index, const char* str);
void str_insertfmt(struct Str* self, size_t index, const char* fmt, ...)
	__attribute__((format (printf, 3, 4)));
void str_append(struct Str* self, const char* str);
void str_appendfmt(struct Str* self, const char* fmt, ...)
	__attribute__((format (printf, 2, 3)));
void str_prepend(struct Str* self, const char* str);
void str_prependfmt(struct Str* self, const char* fmt, ...)
	__attribute__((format (printf, 2, 3)));
void str_dtor(struct Str* self);

#endif
