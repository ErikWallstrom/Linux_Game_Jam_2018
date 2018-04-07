#include "file.h"
#include "log.h"
#include <errno.h>

struct File* file_ctor(
	struct File* self, 
	const char* path, 
	enum FileMode mode
)
{
	log_assert(self, "is NULL");
	log_assert(path, "is NULL");
	log_assert(
		mode & FILEMODE_READ || mode & FILEMODE_WRITE, 
		"invalid mode (%i)",
		mode
	);

	char* m;
	if(mode & FILEMODE_READ && mode & FILEMODE_WRITE)
	{
		m = "r+";
	}
	else if(mode & FILEMODE_READ)
	{
		m = "r";
	}
	else //mode & FILEMODE_WRITE
	{
		//NOTE: 'x' (C11) makes fopen fail if file already exists
		m = "wx";
	}

	self->mode = mode;
	self->raw = fopen(path, m);
	if(!self->raw)
	{
		log_error("%s: '%s'", strerror(errno), path);
	}

	if(mode & FILEMODE_READ)
	{
		if(fseek(self->raw, 0, SEEK_END))
		{
			log_error("%s", strerror(errno));
		}

		long filesize = ftell(self->raw);
		if(filesize == EOF)
		{
			log_error("%s", strerror(errno));
		}

		if(fseek(self->raw, 0, SEEK_SET))
		{
			log_error("%s", strerror(errno));
		}

		self->content = vec_ctor(char, filesize + 1);
		if(filesize)
		{
			vec_expand(self->content, 0, filesize + 1);
			self->content[filesize] = '\0';
			fread(self->content, 1, filesize, self->raw);
		}
		else
		{
			vec_pushback(self->content, '\0');
		}
	}
	else
	{
		self->content = vec_ctor(char, 1);
		vec_pushback(self->content, '\0');
	}

	for(size_t i = 0; i <= strlen(path); i++)
	{
		self->extension = path + i;
		if(path[i] == '.')
		{
			break;
		}
	}

	return self;
}

void file_flush(struct File* self)
{
	log_assert(self, "is NULL");

	if(fseek(self->raw, 0, SEEK_SET))
	{
		log_error("%s", strerror(errno));
	}

	if(fputs(self->content, self->raw) == EOF)
	{
		log_error("%s", strerror(errno));
	}

	if(fflush(self->raw) == EOF)
	{
		log_error("%s", strerror(errno));
	}
}

void file_dtor(struct File* self)
{
	log_assert(self, "is NULL");
	if(self->mode & FILEMODE_WRITE)
	{
		file_flush(self);
	}

	vec_dtor(self->content);
	fclose(self->raw);
}

