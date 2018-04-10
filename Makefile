CC = gcc
WARNINGS = -Wall -Wextra -Wshadow -Wstrict-prototypes -Wdouble-promotion \
		   -Wjump-misses-init -Wnull-dereference -Wrestrict -Wlogical-op \
		   -Wduplicated-branches -Wduplicated-cond
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm
DEBUG_FLAGS = -Og -g3
RELEASE_FLAGS = -march=native -mtune=native -O2 -DNDEBUG
CLIENT_FILES = client.c map.c API2/*.c
SERVER_FILES = server.c API2/log.c API2/ansicode.c

debug:
	$(CC) $(CLIENT_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o build/client $(LIBS)
	$(CC) $(SERVER_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o build/server

release:
	$(CC) $(CLIENT_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o build/client $(LIBS)
	$(CC) $(SERVER_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o build/server
