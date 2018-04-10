#include "map.h"
#include "API2/log.h"

struct Map* map_ctor(struct Map* self, SDL_Renderer* renderer)
{
	log_assert(self, "is NULL");
	log_assert(renderer, "is NULL");
	texture_ctorimage(&self->tilesheet, "tiles.png", renderer);
	self->tiles = vec_ctor(struct Tile, 0);
	self->renderer = renderer;

	for(size_t i = 0; i < TILES_NUM_X; i++)
	{
		for(size_t j = 0; j < TILES_NUM_X; j++)
		{
			int r = rand() % 10000; 
			if(r < 500)
			{
				struct Tile tile;
				rect_ctor(
					&tile.rect, 
					(struct Vec2d){
						i * TILE_WIDTH * TILE_SCALE,
						j * TILE_HEIGHT * TILE_SCALE
					},
					RECTREGPOINT_TOPLEFT,
					TILE_WIDTH * TILE_SCALE,
					TILE_HEIGHT * TILE_SCALE
				);

				tile.type = TILETYPE_STONE;
				vec_pushback(self->tiles, tile);
			}
			else if(r < 1000)
			{
				struct Tile tile;
				rect_ctor(
					&tile.rect, 
					(struct Vec2d){
						i * TILE_WIDTH * TILE_SCALE,
						j * TILE_HEIGHT * TILE_SCALE
					},
					RECTREGPOINT_TOPLEFT,
					TILE_WIDTH * TILE_SCALE,
					TILE_HEIGHT * TILE_SCALE
				);

				tile.type = TILETYPE_TREE;
				vec_pushback(self->tiles, tile);
			}
		}
	}

	return self;
}

void map_render(struct Map* self, double camerax, double cameray) 
{
	log_assert(self, "is NULL");
	for(size_t i = 0; i < TILES_NUM_X; i++)
	{
		for(size_t j = 0; j < TILES_NUM_Y; j++)
		{
			SDL_RenderCopy(
				self->renderer, 
				self->tilesheet.raw,
				&(SDL_Rect){
					TILETYPE_GRASS * TILE_WIDTH,
					0,
					TILE_WIDTH,
					TILE_HEIGHT
				},
				&(SDL_Rect){
					i * TILE_WIDTH *  TILE_SCALE - camerax,
					j * TILE_HEIGHT * TILE_SCALE - cameray,
					TILE_WIDTH *  TILE_SCALE,
					TILE_HEIGHT * TILE_SCALE
				}
			);
		}
	}

	for(size_t i = 0; i < vec_getsize(self->tiles); i++)
	{
		SDL_RenderCopy(
			self->renderer, 
			self->tilesheet.raw,
			&(SDL_Rect){
				self->tiles[i].type * TILE_WIDTH,
				0,
				TILE_WIDTH,
				TILE_HEIGHT
			},
			&(SDL_Rect){
				self->tiles[i].rect.pos.x - camerax,
				self->tiles[i].rect.pos.y - cameray,
				self->tiles[i].rect.width,
				self->tiles[i].rect.height,
			}
		);
	}
}

void map_dtor(struct Map* self)
{
	log_assert(self, "is NULL");
	vec_dtor(self->tiles);
	texture_dtor(&self->tilesheet);
}

