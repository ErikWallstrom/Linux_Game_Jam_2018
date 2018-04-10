#ifndef MAP_H
#define MAP_H

#include "API2/texture.h"
#include "API2/rect.h"
#include "API2/vec.h"

#define TILES_NUM_X 50
#define TILES_NUM_Y 50
#define TILE_SCALE  5
#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define MAP_WIDTH   (TILE_WIDTH * TILE_SCALE * TILES_NUM_X)
#define MAP_HEIGHT  (TILE_HEIGHT * TILE_SCALE * TILES_NUM_Y)

enum TileType
{
	TILETYPE_STONE,
	TILETYPE_TREE,
	TILETYPE_GRASS
};

struct Tile
{
	struct Rect rect;
	enum TileType type;
};

struct Map
{
	struct Texture tilesheet;
	Vec(struct Tile) tiles;
	SDL_Renderer* renderer;
};

struct Map* map_ctor(struct Map* self, SDL_Renderer* renderer);
void map_render(struct Map* self, double camerax, double cameray);
void map_dtor(struct Map* self);

#endif
