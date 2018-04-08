#include "API2/inputhandler.h"
#include "API2/initialize.h"
#include "API2/transition.h"
#include "API2/texture.h"
#include "API2/window.h"
#include "API2/font.h"
#include "API2/rect.h"
#include "API2/log.h"
#include <stdlib.h>
#include <time.h>

#define TICKS_PER_SEC 30.0
#define TICK_RATE (1000.0 / TICKS_PER_SEC)
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16
#define SPRITE_SCALE 5
#define TILES_NUM_X 50
#define TILES_NUM_Y 50
#define MAP_WIDTH (SPRITE_WIDTH * SPRITE_SCALE * TILES_NUM_X)
#define MAP_HEIGHT (SPRITE_HEIGHT * SPRITE_SCALE * TILES_NUM_Y)
#define PLAYER_SPEED (400.0 / TICKS_PER_SEC) 

enum TileLevel
{
	TILELEVEL_GROUND,
	TILELEVEL_ABOVE,
	TILELEVEL_COUNT,
};

enum SpriteSheet
{
	SPRITESHEET_NONE = -1,
	SPRITESHEET_STONE,
	SPRITESHEET_TREE,
	SPRITESHEET_GRASS,
	SPRITESHEET_PLAYER,
	SPRITESHEET_COUNT,
};

struct Tile
{
	enum SpriteSheet type;
};

struct Entity
{
	struct Transition rotate;
	struct Rect rect;
	struct Vec2d oldpos;
	enum SpriteSheet type;
};

struct GameState
{
	struct Window* window;
	struct InputHandler* input;
	struct Texture* spritesheet;
	struct Tile tiles[TILELEVEL_COUNT][TILES_NUM_X][TILES_NUM_Y];
	struct Entity player;
	int* running;
} gamestate;

static void onerror(void* udata)
{
	(void)udata;
	abort();
}

static uint64_t getperformancefreq(void)
{
	return 1000000000; //10^-9 (nano)
}

static uint64_t getperformancecount(void)
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	uint64_t ticks = ts.tv_sec;
	ticks *= getperformancefreq();
	ticks += ts.tv_nsec;

	return ticks;
}

static void update(void)
{
	for(size_t i = 0; i < vec_getsize(gamestate.input->events); i++)
	{
		switch(gamestate.input->events[i].type)
		{
		case SDL_QUIT:
			*gamestate.running = 0;
		default:;
		}
	}

	double speedmod = 1.0;
	gamestate.player.oldpos = gamestate.player.rect.pos;
	if(gamestate.input->keystate[SDL_SCANCODE_Q])
	{
		*gamestate.running = 0;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_LSHIFT]
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 0)
	{
		speedmod = 2.0;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_A]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTX) < 0)
	{
		gamestate.player.rect.pos.x -= PLAYER_SPEED * speedmod;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_D]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTX) > 0)
	{
		gamestate.player.rect.pos.x += PLAYER_SPEED * speedmod;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_W]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_UP)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTY) < 0)
	{
		gamestate.player.rect.pos.y -= PLAYER_SPEED * speedmod;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_S]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTY) > 0)
	{
		gamestate.player.rect.pos.y += PLAYER_SPEED * speedmod;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_B]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_B))
	{
		gamestate.player.rotate.done = 0;
		if(*gamestate.player.rotate.value == gamestate.player.rotate.to)
		{
			*gamestate.player.rotate.value = gamestate.player.rotate.from;
		}
	}

	if(gamestate.player.rect.pos.x < 0)
	{
		gamestate.player.rect.pos.x = 0;
		gamestate.player.oldpos.x = 0;
	}
	if(gamestate.player.rect.pos.y < 0)
	{
		gamestate.player.rect.pos.y = 0;
		gamestate.player.oldpos.y = 0;
	}
	if(gamestate.player.rect.pos.x + gamestate.player.rect.width > MAP_WIDTH)
	{
		gamestate.player.rect.pos.x = MAP_WIDTH - gamestate.player.rect.width;
		gamestate.player.oldpos.x = MAP_WIDTH - gamestate.player.rect.width;
	}
	if(gamestate.player.rect.pos.y + gamestate.player.rect.height > MAP_HEIGHT)
	{
		gamestate.player.rect.pos.y = MAP_HEIGHT - gamestate.player.rect.height;
		gamestate.player.oldpos.y = MAP_HEIGHT - gamestate.player.rect.height;
	}
}

static void render(double interpolation, double delta)
{
	double rendercamerax = gamestate.player.oldpos.x 
		+ (gamestate.player.rect.pos.x - gamestate.player.oldpos.x) 
		* interpolation - gamestate.window->width / 2.0
		+ gamestate.player.rect.width / 2.0;
	double rendercameray = gamestate.player.oldpos.y
		+ (gamestate.player.rect.pos.y - gamestate.player.oldpos.y)
		* interpolation - gamestate.window->height / 2.0 
		+ gamestate.player.rect.height / 2.0;

	if(rendercamerax < 0.0)
	{
		rendercamerax = 0.0;
	}
	if(rendercameray < 0.0)
	{
		rendercameray = 0.0;
	}
	if(rendercamerax + gamestate.window->width > MAP_WIDTH)
	{
		rendercamerax = MAP_WIDTH - gamestate.window->width;
	}
	if(rendercameray + gamestate.window->height > MAP_HEIGHT)
	{
		rendercameray = MAP_HEIGHT - gamestate.window->height;
	}

	for(size_t k = 0; k < TILELEVEL_COUNT; k++)
	{
		for(size_t i = 0; i < TILES_NUM_X; i++)
		{
			for(size_t j = 0; j < TILES_NUM_Y; j++)
			{
				if(gamestate.tiles[k][i][j].type != SPRITESHEET_NONE)
				{
					SDL_RenderCopy(
						gamestate.window->renderer,
						gamestate.spritesheet->raw,
						&(SDL_Rect){
							gamestate.tiles[k][i][j].type * SPRITE_WIDTH,
							0,
							SPRITE_WIDTH,
							SPRITE_HEIGHT
						},
						&(SDL_Rect){
							i * SPRITE_WIDTH * SPRITE_SCALE - rendercamerax,
							j * SPRITE_HEIGHT * SPRITE_SCALE - rendercameray,
							SPRITE_WIDTH * SPRITE_SCALE,
							SPRITE_HEIGHT * SPRITE_SCALE
						}
					);
				}
			}
		}
	}

	SDL_RenderCopyEx(
		gamestate.window->renderer,
		gamestate.spritesheet->raw,
		&(SDL_Rect){
			gamestate.player.type * SPRITE_WIDTH,
			0,
			SPRITE_WIDTH,
			SPRITE_HEIGHT
		},
		&(SDL_Rect){
			gamestate.player.oldpos.x + (gamestate.player.rect.pos.x
				- gamestate.player.oldpos.x) * interpolation - rendercamerax, 
			gamestate.player.oldpos.y + (gamestate.player.rect.pos.y
				- gamestate.player.oldpos.y) * interpolation - rendercameray,  
			gamestate.player.rect.width,
			gamestate.player.rect.height
		},
		*gamestate.player.rotate.value/*atan2(
			SDL_GameControllerGetAxis(
				gamestate.input->controller, 
				SDL_CONTROLLER_AXIS_RIGHTY
			), 
			SDL_GameControllerGetAxis(
				gamestate.input->controller, 
				SDL_CONTROLLER_AXIS_RIGHTX
			)
		) * ( 180.0 / M_PI ) + 90.0*/,
		NULL,
		SDL_FLIP_NONE
	);

	transition_update(&gamestate.player.rotate, delta);
	SDL_SetRenderDrawColor(gamestate.window->renderer, 0, 0, 0, 255);
}

int main(void)
{
	log_seterrorhandler(onerror, NULL);
	initialize();

	struct Window window;
	window_ctor(&window, "Game Window", 1600, 900, WINDOW_DEFAULT);

	struct InputHandler input;
	inputhandler_ctor(&input);

	struct Texture spritesheet;
	texture_ctorimage(&spritesheet, "sprites.png", window.renderer);

	struct Rect rect;
	rect_ctor(
		&rect, 
		(struct Vec2d){
			window.width / 2.0, 
			window.height / 2.0
		},
		RECTREGPOINT_CENTER,
		spritesheet.width,
		spritesheet.height
	);

	int running = 1;
	gamestate.running = &running;
	gamestate.spritesheet = &spritesheet;
	gamestate.window = &window;
	gamestate.input = &input;
	gamestate.player.type = SPRITESHEET_PLAYER;
	rect_ctor(
		&gamestate.player.rect, 
		(struct Vec2d){window.width / 2.0, window.height / 2.0},
		RECTREGPOINT_CENTER,
		SPRITE_WIDTH * SPRITE_SCALE,
		SPRITE_HEIGHT * SPRITE_SCALE
	);

	double value;
	transition_ctor(
		&gamestate.player.rotate, 
		TRANSITIONTYPE_DEFAULT, 
		0, 
		360, 
		500, 
		&value
	);
	gamestate.player.rotate.done = 1;

	for(size_t i = 0; i < TILES_NUM_X; i++)
	{
		for(size_t j = 0; j < TILES_NUM_Y; j++)
		{
			gamestate.tiles[TILELEVEL_GROUND][i][j].type = SPRITESHEET_GRASS;
		}
	}

	for(size_t i = 0; i < TILES_NUM_X; i++)
	{
		for(size_t j = 0; j < TILES_NUM_Y; j++)
		{
			enum SpriteSheet type;
			int r = rand() % 10000; 
			if(r < 9000)
			{
				type = SPRITESHEET_NONE;
			}
			else if(r < 9600)
			{
				type = SPRITESHEET_TREE;
			}
			else
			{
				type = SPRITESHEET_STONE;
			}

			gamestate.tiles[TILELEVEL_ABOVE][i][j].type = type;
		}
	}

	uint64_t oldtime = getperformancecount();
	double lag = 0.0;

	unsigned ticks = 0;
	unsigned secs = 0;
	while(running)
	{
		uint64_t curtime = getperformancecount();
		double delta = (curtime - oldtime) * 1000.0 / getperformancefreq(); //ms
		oldtime = curtime;
		lag += delta;

		while(lag >= TICK_RATE)
		{
			ticks++;
			lag -= TICK_RATE;
			update();
			inputhandler_update(&input);
		}

		double interpolation = lag / TICK_RATE;
		render(interpolation, delta);
		window_render(&window);

		if(secs != window.seconds)
		{
			log_info("Ticks: %u, FPS: %u", ticks, window.fps);
			secs = window.seconds;
			ticks = 0;
		}
	}

	texture_dtor(&spritesheet);
	inputhandler_dtor(&input);
	window_dtor(&window);
	cleanup();
}
