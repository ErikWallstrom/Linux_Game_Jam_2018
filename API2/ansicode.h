#ifndef ANSICODES_H
#define ANSICODES_H

#include <stdarg.h>

struct ANSICodeFg 
{
	const char* name;
};

struct ANSICodeBg
{
	const char* name;
};

struct ANSICode
{
	const struct ANSICodeFg* fg;
	const struct ANSICodeBg* bg;
	int bold;
	int italic;
	int underline;
};

extern const struct ANSICodeFg* const ANSICODE_FG_BLACK;
extern const struct ANSICodeFg* const ANSICODE_FG_RED;
extern const struct ANSICodeFg* const ANSICODE_FG_GREEN;
extern const struct ANSICodeFg* const ANSICODE_FG_YELLOW;
extern const struct ANSICodeFg* const ANSICODE_FG_BLUE;
extern const struct ANSICodeFg* const ANSICODE_FG_MAGENTA;
extern const struct ANSICodeFg* const ANSICODE_FG_CYAN;
extern const struct ANSICodeFg* const ANSICODE_FG_WHITE;
extern const struct ANSICodeFg* const ANSICODE_FG_DEFAULT;

extern const struct ANSICodeBg* const ANSICODE_BG_BLACK;
extern const struct ANSICodeBg* const ANSICODE_BG_RED;
extern const struct ANSICodeBg* const ANSICODE_BG_GREEN;
extern const struct ANSICodeBg* const ANSICODE_BG_YELLOW;
extern const struct ANSICodeBg* const ANSICODE_BG_BLUE;
extern const struct ANSICodeBg* const ANSICODE_BG_MAGENTA;
extern const struct ANSICodeBg* const ANSICODE_BG_CYAN;
extern const struct ANSICodeBg* const ANSICODE_BG_WHITE;
extern const struct ANSICodeBg* const ANSICODE_BG_DEFAULT;

void ansicode_printf(struct ANSICode* self, const char* fmt, ...)
	__attribute__((format(printf, 2, 3)));
void ansicode_vprintf(struct ANSICode* self, const char* fmt, va_list args)
	__attribute__((format(printf, 2, 0)));

#endif
