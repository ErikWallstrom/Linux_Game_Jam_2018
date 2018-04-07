#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "vec.h"

enum FileMode
{
	FILEMODE_READ = 1 << 0,
	FILEMODE_WRITE = 1 << 1,
};

struct File
{
	FILE* raw;
	Vec(char) content;
	//NOTE: 'extension' simply points to path, no memory is allocated
	const char* extension;
	enum FileMode mode;
	//Should file name be included?
};

struct File* file_ctor(
	struct File* self, 
	const char* path, 
	enum FileMode mode
);
void file_flush(struct File* self);
void file_dtor(struct File* self);

#endif
