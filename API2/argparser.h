#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <stddef.h>
#include "vec.h"

struct ArgParserLongOpt
{ 
	const char* opt;
	const char* info;
	int hasarg;
};

struct ArgParserResult
{ 
	const char* arg;
	int used;
};

struct ArgParser
{ 
	Vec(struct ArgParserResult) results;
	struct ArgParserLongOpt* opts;
	char** argv;
	size_t numopts;
};

struct ArgParser* argparser_ctor(
	struct ArgParser* self, 
	int argc,
	char** argv,
	struct ArgParserLongOpt* opts, 
	size_t numopts
);
void argparser_printhelp(struct ArgParser* self);
void argparser_dtor(struct ArgParser* self);

#endif
