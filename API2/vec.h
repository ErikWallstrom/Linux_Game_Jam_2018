#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <string.h>

#define Vec(T) T*

//Function wrappers
#define vec_ctor(T, n) \
	(T*)vec_ctor_(sizeof(T), (n))
#define vec_dtor(v) \
	vec_dtor_(&(v))
#define vec_getsize(v) \
	vec_getsize_(&(v))
#define vec_expand(v, p, e) \
	vec_expand_(&(v), (p), (e))
#define vec_collapse(v, p, e) \
	vec_collapse_(&(v), (p), (e))

//XXX: Extra functionality, should probably be real functions
#define vec_insert(v, p, ...) \
	vec_expand((v), (p), 1), (v)[p] = __VA_ARGS__
#define vec_remove(v, p) \
	vec_collapse((v), (p), 1)
#define vec_push(v, ...) \
	vec_insert((v), 0, __VA_ARGS__)
#define vec_pushback(v, ...) \
	vec_expand((v), vec_getsize(v), 1), \
	(v)[vec_getsize(v) - 1] = __VA_ARGS__
#define vec_pop(v) \
	vec_collapse((v), 0, 1)
#define vec_popback(v) \
	vec_collapse((v), vec_getsize(v) - 1, 1)

#define vec_set(v, a, n) \
	vec_collapse((v), 0, vec_getsize((v))), \
	vec_expand((v), 0, (n)), \
	memcpy((v), (a), (n) * sizeof(*(a)))
#define vec_clear(v) \
	vec_collapse((v), 0, vec_getsize((v)))
//XXX: vec_pushbackwitharr is ugly
#define vec_pushbackwitharr(v, a, n) \
	vec_expand((v), vec_getsize((v)), (n)), \
	memcpy((v) + vec_getsize((v)) - (n), (a), (n) * sizeof(*(a)))
#define vec_pushwitharr(v, a, n) \
	vec_expand((v), 0, (n)), \
	memcpy((v), (a), (n) * sizeof(*(a)))

Vec(void) vec_ctor_(size_t elementsize, size_t elements);
void vec_dtor_(Vec(void) vec);
size_t vec_getsize_(Vec(void) vec);
void vec_expand_(Vec(void) vec, size_t pos, size_t elements);
void vec_collapse_(Vec(void) vec, size_t pos, size_t elements);

#endif
