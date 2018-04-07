CC = gcc
WARNINGS = -Wall -Wextra -Wshadow -Wstrict-prototypes -Wdouble-promotion \
		   -Wjump-misses-init -Wnull-dereference -Wrestrict -Wlogical-op \
		   -Wduplicated-branches -Wduplicated-cond
FILES = *.c API2/*.c
EXECUTABLE = a.out
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

debug:
	$(CC) $(FILES) $(WARNINGS) -Og -g3 -o $(EXECUTABLE) $(LIBS)

release:
	$(CC) $(FILES) $(WARNINGS) -march=native -mtune=native -O2 -DNDEBUG -o \
	$(EXECUTABLE) $(LIBS)
