#include "API2/inputhandler.h"
#include "API2/initialize.h"
#include "API2/transition.h"
#include "API2/texture.h"
#include "API2/window.h"
#include "API2/font.h"
#include "API2/rect.h"
#include "API2/log.h"
#include "API2/str.h"
#include "projectile.h"
#include "player.h"
#include "enemy.h"
#include "map.h"
/*
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
*/
#include <time.h>

#define TICKS_PER_SEC 30.0
const double TICK_RATE = 1000.0 / TICKS_PER_SEC;
#define CONTROLLER_MAX  32767
#define CONTROLLER_MIN -32768
#define DASHFACTOR 10.0
#define MAXCHARGE 100.0
#define CHARGESPEED (MAXCHARGE / (TICK_RATE * 1.0))
#define CHARGETRANSITIONDURATION 200

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 675

#define HPBAR_X      25
#define HPBAR_Y      15
#define HPBAR_WIDTH  250
#define HPBAR_HEIGHT 25

#define NUM_STAGES 7

#define ONSCREENTEXT_Y 30
#define TIMER_Y 30
#define TIMER_X (WINDOW_WIDTH - 50)

enum GameState
{
	GAMESTATE_RUNNING,
	GAMESTATE_GAMEOVER,
	GAMESTATE_WIN,
	GAMESTATE_QUIT
};

struct Game
{
	struct Texture onscreentexts[NUM_STAGES];
	struct Transition chargetransition;
	struct Transition timer;
	struct Texture timertext;
	struct Window* window;
	struct InputHandler* input;
	struct Texture* spritesheet;
	struct Map* map;
	struct Player* player;
	struct Projectiles* projectiles;
	struct Enemies* enemies;
	enum GameState* state;
	struct Font* font;
	double shootcharge;
	double dashcharge;
	int shootcharging;
	int dashcharging;
	int stage;
} gamestate;

static int stagesdone[NUM_STAGES];
int slimekills = 0;
int ghostkills = 0;
int touchedtop = 0;
int touchedbottom = 0;
static int gotsuper = 0;

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

void shoot(void)
{
	struct Vec2d pos = rect_getpos(
		&gamestate.player->rect,
		RECTREGPOINT_CENTER
	);

	double rotation = 0.0;
	if(gamestate.player->direction.x == 0.0 
		&& gamestate.player->direction.y == 0.0)
	{
		switch(gamestate.player->selectedanimation)
		{
		case PLAYERSTANDING_LEFT:
			rotation = M_PI;
			break;

		case PLAYERSTANDING_RIGHT:
			rotation = 0.0;
			break;

		case PLAYERSTANDING_FRONT:
			rotation = M_PI / 2.0;
			break;

		case PLAYERSTANDING_BACK:
			rotation = M_PI * 1.5;
			break;
		}
	}
	else
	{
		rotation = atan2(
			gamestate.player->direction.y,
			gamestate.player->direction.x
		);
	}

	if(gamestate.shootcharge == MAXCHARGE || gotsuper)
	{
		projectiles_add(
			gamestate.projectiles, 
			pos, 
			rotation - (M_PI / 20.0) * 2,
			PROJECTILETYPE_SPECIAL
		);
		projectiles_add(
			gamestate.projectiles, 
			pos, 
			rotation - (M_PI / 20.0) * 1,
			PROJECTILETYPE_SPECIAL
		);
		projectiles_add(
			gamestate.projectiles, 
			pos, 
			rotation,
			PROJECTILETYPE_SPECIAL
		);
		projectiles_add(
			gamestate.projectiles, 
			pos, 
			rotation + (M_PI / 20.0) * 1,
			PROJECTILETYPE_SPECIAL
		);
		projectiles_add(
			gamestate.projectiles, 
			pos, 
			rotation + (M_PI / 20.0) * 2,
			PROJECTILETYPE_SPECIAL
		);
	}
	else
	{
		projectiles_add(
			gamestate.projectiles, 
			pos, 
			rotation,
			PROJECTILETYPE_NORMAL
		);
	}
}

static void update(void)
{
	switch(gamestate.stage)
	{
	case 0:
		{
			if(!stagesdone[gamestate.stage])
			{
				enemies_slimes(gamestate.enemies, 25);
			}

			struct Str text;
			str_ctorfmt(&text, "%i", 10 - slimekills);
			texture_dtor(&gamestate.timertext);
			texture_ctortext(
				&gamestate.timertext, 
				gamestate.font, 
				text.data,
				gamestate.window->renderer
			);
			str_dtor(&text);

			stagesdone[gamestate.stage] = 1;
			if(slimekills >= 10)
			{
				gamestate.stage++;
			}
		}
		break;

	case 1:
		{
			if(!stagesdone[gamestate.stage])
			{
				enemies_ghosts(gamestate.enemies, 25);
			}

			struct Str text;
			str_ctorfmt(&text, "%i", 10 - ghostkills);
			texture_dtor(&gamestate.timertext);
			texture_ctortext(
				&gamestate.timertext, 
				gamestate.font, 
				text.data,
				gamestate.window->renderer
			);
			str_dtor(&text);

			stagesdone[gamestate.stage] = 1;
			if(ghostkills >= 10)
			{
				gamestate.stage++;
			}
		}
		break;

	case 2:
		{
			if(!stagesdone[gamestate.stage])
			{
				touchedtop = 0;
				gamestate.timer.done = 0;
			}

			struct Str text;
			str_ctorfmt(&text, "%i", 13 - ((int)*gamestate.timer.value) / 1000);
			texture_dtor(&gamestate.timertext);
			texture_ctortext(
				&gamestate.timertext, 
				gamestate.font, 
				text.data,
				gamestate.window->renderer
			);
			str_dtor(&text);
			
			stagesdone[gamestate.stage] = 1;
			if(gamestate.timer.done)
			{
				gamestate.player->hp = 0;
			}

			if(touchedtop)
			{
				gamestate.stage++;
			}
		}
		break;

	case 3:
		{
			if(!stagesdone[gamestate.stage])
			{
				touchedbottom = 0;
				*gamestate.timer.value = 0.0;
				gamestate.timer.done = 0;
			}
			
			struct Str text;
			str_ctorfmt(&text, "%i", 13 - ((int)*gamestate.timer.value) / 1000);
			texture_dtor(&gamestate.timertext);
			texture_ctortext(
				&gamestate.timertext, 
				gamestate.font, 
				text.data,
				gamestate.window->renderer
			);

			str_dtor(&text);
			stagesdone[gamestate.stage] = 1;
			if(gamestate.timer.done)
			{
				gamestate.player->hp = 0;
			}

			if(touchedbottom)
			{
				gamestate.stage++;
			}
		}
		break;

	case 4:
		{
			if(!stagesdone[gamestate.stage])
			{
				transition_ctor(
					&gamestate.timer, 
					TRANSITIONTYPE_DEFAULT,
					0,
					25000,
					25000,
					gamestate.timer.value
				);
				enemies_slimes(gamestate.enemies, 90);
				enemies_ghosts(gamestate.enemies, 90);
			}

			struct Str text;
			str_ctorfmt(&text, "%i", 25 - ((int)*gamestate.timer.value) / 1000);
			texture_dtor(&gamestate.timertext);
			texture_ctortext(
				&gamestate.timertext, 
				gamestate.font, 
				text.data,
				gamestate.window->renderer
			);
			str_dtor(&text);
			
			stagesdone[gamestate.stage] = 1;
			if(gamestate.timer.done)
			{
				gamestate.stage++;
				for(size_t i = 0; i < 180; i++)
				{
					vec_popback(gamestate.enemies->enemies);
				}
			}
		}
		break;

	case 5:
		{
			if(!stagesdone[gamestate.stage])
			{
			
			}

			stagesdone[gamestate.stage] = 1;
			if(1)
			{
				gamestate.stage++;
			}
		}
		break;

	case 6:
		{
			if(!stagesdone[gamestate.stage])
			{
				enemies_terrain(gamestate.enemies, gamestate.map);
				gotsuper = 1;
			}

			size_t numalive = 0;
			for(size_t i = 0; i < vec_getsize(gamestate.enemies->enemies); i++)
			{
				if(gamestate.enemies->enemies[i].hp > 0)
				{
					numalive++;
				}
			}

			struct Str text;
			str_ctorfmt(&text, "%zu", numalive);
			texture_dtor(&gamestate.timertext);
			texture_ctortext(
				&gamestate.timertext, 
				gamestate.font, 
				text.data,
				gamestate.window->renderer
			);

			str_dtor(&text);
			stagesdone[gamestate.stage] = 1;
			if(numalive == 0)
			{
				gamestate.stage++;
			}
		}
		break;

	default:
		*gamestate.state = GAMESTATE_WIN;
		return;
	}

	struct Vec2d movement = {0};
	if(gamestate.input->keystate[SDL_SCANCODE_A]
		|| gamestate.input->keystate[SDL_SCANCODE_H] 
		|| gamestate.input->keystate[SDL_SCANCODE_LEFT] 
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_LEFT))
	{
		movement.x = -PLAYER_SPEED;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_D]
		|| gamestate.input->keystate[SDL_SCANCODE_L] 
		|| gamestate.input->keystate[SDL_SCANCODE_RIGHT] 
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
	{
		movement.x = PLAYER_SPEED;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_W]
		|| gamestate.input->keystate[SDL_SCANCODE_K] 
		|| gamestate.input->keystate[SDL_SCANCODE_UP] 
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_UP)
		)
	{
		movement.y = -PLAYER_SPEED;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_S]
		|| gamestate.input->keystate[SDL_SCANCODE_J] 
		|| gamestate.input->keystate[SDL_SCANCODE_DOWN] 
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		)
	{
		movement.y = PLAYER_SPEED;
	}

	if(SDL_GameControllerGetAxis(
			gamestate.input->controller, 
			SDL_CONTROLLER_AXIS_LEFTX) 
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller, 
			SDL_CONTROLLER_AXIS_LEFTY)
		)
	{ 
		double angle = atan2(
			SDL_GameControllerGetAxis(
				gamestate.input->controller, 
				SDL_CONTROLLER_AXIS_LEFTY
			), 
			SDL_GameControllerGetAxis(
				gamestate.input->controller, 
				SDL_CONTROLLER_AXIS_LEFTX
			)
		);

		movement.x = cos(angle) * PLAYER_SPEED;
		movement.y = sin(angle) * PLAYER_SPEED;
	}

	vec2d_normalize(&movement, &gamestate.player->direction);
	for(size_t i = 0; i < vec_getsize(gamestate.input->events); i++)
	{
		switch(gamestate.input->events[i].type)
		{
		case SDL_CONTROLLERAXISMOTION:
			if(gamestate.input->events[i].caxis.axis 
				== SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
			{
				if(gamestate.input->events[i].caxis.value == CONTROLLER_MAX)
				{
					if(gamestate.player->force.x == 0.0
						&& gamestate.player->force.y == 0.0)
					{
						switch(gamestate.player->selectedanimation)
						{
						case PLAYERSTANDING_LEFT:
						case PLAYERWALKING_LEFT:
							gamestate.player->force.x = -DASHFACTOR
								* PLAYER_DASHDECAY;
							break;

						case PLAYERSTANDING_RIGHT:
						case PLAYERWALKING_RIGHT:
							gamestate.player->force.x = DASHFACTOR
								* PLAYER_DASHDECAY;
							break;

						case PLAYERSTANDING_FRONT:
						case PLAYERWALKING_DOWN:
							gamestate.player->force.y = DASHFACTOR
								* PLAYER_DASHDECAY;
							break;

						case PLAYERSTANDING_BACK:
						case PLAYERWALKING_UP:
							gamestate.player->force.y = -DASHFACTOR
								* PLAYER_DASHDECAY;
							break;
						}

					}
					gamestate.dashcharging = 1;
				}
				else if(gamestate.input->events[i].caxis.value == 0)
				{
					if(gamestate.player->force.x == 0.0
						&& gamestate.player->force.y == 0.0)
					{
						double chargefactor = (gamestate.dashcharge
							== MAXCHARGE) 
							? 1.5
							: 1.0;
						switch(gamestate.player->selectedanimation)
						{
						case PLAYERSTANDING_LEFT:
						case PLAYERWALKING_LEFT:
							gamestate.player->force.x = -DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;

						case PLAYERSTANDING_RIGHT:
						case PLAYERWALKING_RIGHT:
							gamestate.player->force.x = DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;

						case PLAYERSTANDING_FRONT:
						case PLAYERWALKING_DOWN:
							gamestate.player->force.y = DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;

						case PLAYERSTANDING_BACK:
						case PLAYERWALKING_UP:
							gamestate.player->force.y = -DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;
						}
					}

					gamestate.dashcharging = 0;
					gamestate.dashcharge = 0;
				}
			}
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			switch(gamestate.input->events[i].cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_B:
					gamestate.shootcharging = 1;
					break;

				default:;
			}
			break;

		case SDL_CONTROLLERBUTTONUP:
			switch(gamestate.input->events[i].cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_B:
					shoot();
					gamestate.shootcharging = 0;
					gamestate.shootcharge = 0;
					break;

				default:;
			}
			break;

		case SDL_KEYDOWN:
			switch(gamestate.input->events[i].key.keysym.sym)
			{
			case SDLK_q:
				*gamestate.state = GAMESTATE_QUIT;
				break;

			case SDLK_r:
				*gamestate.state = GAMESTATE_WIN;
				break;

			case SDLK_e:
				*gamestate.state = GAMESTATE_GAMEOVER;
				break;

			case SDLK_o:
				enemies_terrain(gamestate.enemies, gamestate.map);
				break;

			case SDLK_p:
				enemies_slimes(gamestate.enemies, 25);
				break;

			case SDLK_i:
				enemies_ghosts(gamestate.enemies, 25);
				break;

			case SDLK_LSHIFT:
				if(gamestate.player->force.x == 0.0
					&& gamestate.player->force.y == 0.0)
				{
					switch(gamestate.player->selectedanimation)
					{
					case PLAYERSTANDING_LEFT:
					case PLAYERWALKING_LEFT:
						gamestate.player->force.x = -DASHFACTOR
							* PLAYER_DASHDECAY;
						break;

					case PLAYERSTANDING_RIGHT:
					case PLAYERWALKING_RIGHT:
						gamestate.player->force.x = DASHFACTOR 
							* PLAYER_DASHDECAY;
						break;

					case PLAYERSTANDING_FRONT:
					case PLAYERWALKING_DOWN:
						gamestate.player->force.y = DASHFACTOR 
							* PLAYER_DASHDECAY;
						break;

					case PLAYERSTANDING_BACK:
					case PLAYERWALKING_UP:
						gamestate.player->force.y = -DASHFACTOR 
							* PLAYER_DASHDECAY;
						break;
					}
				}

				gamestate.dashcharging = 1;
				break;

			case SDLK_SPACE:
				gamestate.shootcharging = 1;
				break;

			default:;
			}
			break;

		case SDL_KEYUP:
			switch(gamestate.input->events[i].key.keysym.sym)
			{
				case SDLK_LSHIFT:
					if(gamestate.player->force.x == 0.0
						&& gamestate.player->force.y == 0.0)
					{
						double chargefactor = (gamestate.dashcharge
							== MAXCHARGE) ? 1.5 : 1.0;
						switch(gamestate.player->selectedanimation)
						{
						case PLAYERSTANDING_LEFT:
						case PLAYERWALKING_LEFT:
							gamestate.player->force.x = -DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;

						case PLAYERSTANDING_RIGHT:
						case PLAYERWALKING_RIGHT:
							gamestate.player->force.x = DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;

						case PLAYERSTANDING_FRONT:
						case PLAYERWALKING_DOWN:
							gamestate.player->force.y = DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;

						case PLAYERSTANDING_BACK:
						case PLAYERWALKING_UP:
							gamestate.player->force.y = -DASHFACTOR
								* PLAYER_DASHDECAY * chargefactor;
							break;
						}
					}

					gamestate.dashcharging = 0;
					gamestate.dashcharge = 0;
					break;

				case SDLK_SPACE:
					shoot();
					gamestate.shootcharging = 0;
					gamestate.shootcharge = 0;
					break;

				default:;
			}
			break;

		case SDL_QUIT:
			*gamestate.state = GAMESTATE_QUIT;

		default:;
		}
	}

	if(gamestate.dashcharging)
	{
		gamestate.dashcharge += CHARGESPEED;
		if(gamestate.dashcharge > MAXCHARGE)
		{
			gamestate.dashcharge = MAXCHARGE;
		}
	}

	if(gamestate.shootcharging)
	{
		gamestate.shootcharge += CHARGESPEED;
		if(gamestate.shootcharge > MAXCHARGE)
		{
			gamestate.shootcharge = MAXCHARGE;
		}
	}

	projectiles_update(gamestate.projectiles, gamestate.map);
	enemies_update(
		gamestate.enemies, 
		gamestate.map, 
		gamestate.player, 
		gamestate.projectiles
	);
	player_update(gamestate.player, gamestate.map);

	if(gamestate.player->hp <= 0)
	{
		*gamestate.state = GAMESTATE_GAMEOVER;
	}
}

static void render(double interpolation, double delta)
{
	double camerax = gamestate.player->oldpos.x 
		+ (gamestate.player->rect.pos.x - gamestate.player->oldpos.x) 
		* interpolation - gamestate.window->width / 2.0
		+ gamestate.player->rect.width / 2.0;
	double cameray = gamestate.player->oldpos.y
		+ (gamestate.player->rect.pos.y - gamestate.player->oldpos.y)
		* interpolation - gamestate.window->height / 2.0 
		+ gamestate.player->rect.height / 2.0;

	if(camerax < 0.0)
	{
		camerax = 0.0;
	}
	if(cameray < 0.0)
	{
		cameray = 0.0;
	}
	if(camerax + gamestate.window->width > MAP_WIDTH)
	{
		camerax = MAP_WIDTH - gamestate.window->width;
	}
	if(cameray + gamestate.window->height > MAP_HEIGHT)
	{
		cameray = MAP_HEIGHT - gamestate.window->height;
	}

	transition_update(&gamestate.chargetransition, delta);
	transition_update(&gamestate.timer, delta);
	if(gamestate.dashcharge == MAXCHARGE || gamestate.shootcharge == MAXCHARGE)
	{
		SDL_SetTextureColorMod(
			gamestate.player->spritesheet.raw, 
			(gamestate.shootcharge == MAXCHARGE) 
				? *gamestate.chargetransition.value
				: 64,
			(gamestate.dashcharge == MAXCHARGE) 
				? *gamestate.chargetransition.value 
				: 64, 
			64
		);
	}

	if(gamestate.dashcharge == MAXCHARGE)
	{
		gamestate.chargetransition.done = 0;
	}
	else if(gamestate.shootcharge == MAXCHARGE)
	{
		gamestate.chargetransition.done = 0;
	}
	else
	{
		*gamestate.chargetransition.value = 0.0;
		gamestate.chargetransition.done = 1;
		SDL_SetTextureColorMod(
			gamestate.player->spritesheet.raw, 
			255, 
			255, 
			255
		);
	}

	map_render(gamestate.map, camerax, cameray);
	enemies_render(gamestate.enemies, interpolation, camerax, cameray);
	projectiles_render(gamestate.projectiles, interpolation, camerax, cameray);
	player_render(gamestate.player, interpolation, delta, camerax, cameray);

	SDL_RenderCopy(
		gamestate.window->renderer, 
		gamestate.onscreentexts[gamestate.stage].raw, 
		NULL,
		&(SDL_Rect){
			WINDOW_WIDTH / 2.0 - gamestate.onscreentexts[gamestate.stage].width
				/ 2.0,
			ONSCREENTEXT_Y,
			gamestate.onscreentexts[gamestate.stage].width,
			gamestate.onscreentexts[gamestate.stage].height
		}
	);

	SDL_SetRenderDrawColor(gamestate.window->renderer, 50, 50, 50, 255);
	SDL_RenderFillRect(
		gamestate.window->renderer, 
		&(SDL_Rect){
			HPBAR_X, 
			HPBAR_Y, 
			HPBAR_WIDTH,
			HPBAR_HEIGHT
		}
	);
	SDL_SetRenderDrawColor(gamestate.window->renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(
		gamestate.window->renderer, 
		&(SDL_Rect){
			HPBAR_X, 
			HPBAR_Y, 
			HPBAR_WIDTH * (gamestate.player->oldhp / 100.0), 
			HPBAR_HEIGHT
		}
	);

	SDL_SetRenderDrawColor(gamestate.window->renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(
		gamestate.window->renderer, 
		&(SDL_Rect){HPBAR_X, HPBAR_Y, HPBAR_WIDTH, HPBAR_HEIGHT}
	);

	SDL_RenderCopy(
		gamestate.window->renderer, 
		gamestate.timertext.raw,
		NULL,
		&(SDL_Rect){
			TIMER_X - gamestate.timertext.width / 2.0,
			TIMER_Y,
			gamestate.timertext.width,
			gamestate.timertext.height,
		}
	);
}

int main(int argc, char* argv[])
{
	log_seterrorhandler(onerror, NULL);
	/* if(argc < 3)
	{
		printf("Usage: %s [IP] [PORT]\n", argv[0]);
		return EXIT_FAILURE;
	}

	signal(SIGPIPE, SIG_IGN);
	int clientsocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientsocket < 0)
	{
		log_error("%s", strerror(errno));
	}

	int result = setsockopt(
		clientsocket, 
		IPPROTO_TCP, 
		TCP_NODELAY, 
		&(int){1},
		sizeof(int)
	);
	if(result < 0)
	{
		log_error("%s", strerror(errno));
	}

	struct sockaddr_in serveraddr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_zero = {0} //Padding
	};

	result = inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);
	if(result == 0)
	{
		log_error("Invalid IPv4 Address");
	}
	else if(result == -1)
	{
		log_error("%s", strerror(errno));
	}

	result = connect(
		clientsocket, 
		(struct sockaddr*)&serveraddr, 
		sizeof serveraddr
	);
	if(result < 0)
	{
		log_error("%s", strerror(errno));
	} */

	initialize();

	struct Window window;
	window_ctor(
		&window, 
		"Game Window", 
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		WINDOW_VSYNC | WINDOW_FULLSCREEN
	);

	struct InputHandler input;
	inputhandler_ctor(&input);

	struct Texture spritesheet;
	texture_ctorimage(&spritesheet, "resources/tiles.png", window.renderer);

	struct Map map;
	map_ctor(&map, window.renderer);

	struct Player player;
	player_ctor(
		&player, 
		(struct Vec2d){MAP_WIDTH / 2.0, MAP_HEIGHT / 2.0},
		window.renderer
	);

	struct Projectiles projectiles;
	projectiles_ctor(&projectiles, window.renderer);

	struct Enemies enemies;
	enemies_ctor(&enemies, window.renderer);

	struct Font stagefont;
	font_ctor(
		&stagefont, 
		"resources/font.ttf", 
		24,
		(SDL_Color){255, 255, 255, 255}, 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[0], 
		&stagefont,
		"Kill 10 Slimes!", 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[1], 
		&stagefont,
		"Kill 10 Ghosts!", 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[2], 
		&stagefont,
		"Reach the top of the map within 13 seconds!", 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[3], 
		&stagefont,
		"Reach the bottom of the map within 13 seconds!", 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[4], 
		&stagefont,
		"Survive 25 seconds!", 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[5], 
		&stagefont,
		"Defeat the boss!", 
		window.renderer
	);

	texture_ctortext(
		&gamestate.onscreentexts[6], 
		&stagefont,
		"Kill everything!", 
		window.renderer
	);

	enum GameState state = GAMESTATE_RUNNING;
	gamestate.state = &state;
	gamestate.spritesheet = &spritesheet;
	gamestate.window = &window;
	gamestate.input = &input;
	gamestate.map = &map;
	gamestate.player = &player;
	gamestate.projectiles = &projectiles;
	gamestate.enemies = &enemies;
	gamestate.font = &stagefont;

	double value1 = 0.0;
	transition_ctor(
		&gamestate.chargetransition, 
		TRANSITIONTYPE_FADE,
		0,
		255,
		CHARGETRANSITIONDURATION,
		&value1
	);

	double value2 = 0.0;
	transition_ctor(
		&gamestate.timer, 
		TRANSITIONTYPE_DEFAULT,
		0,
		13000,
		13000,
		&value2
	);
	gamestate.timer.done = 1;
	gamestate.stage = 5;
	texture_ctortext(&gamestate.timertext, &stagefont, " ", window.renderer);

	uint64_t oldtime = getperformancecount();
	unsigned ticks = 0;
	double lag = 0.0;
	while(state == GAMESTATE_RUNNING)
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

#ifndef NDEBUG
		static unsigned secs = 0;
		if(secs != window.seconds)
		{
			/*
			char buffer[12] = "Hello world";
			result = write(clientsocket, buffer, sizeof buffer);
			if(result < 0)
			{
				log_warning("Lost connection to server (%s)", strerror(errno));
				break;
			}
			*/
			
			log_info("Ticks: %u, FPS: %u", ticks, window.fps);
			secs = window.seconds;
			ticks = 0;
		}
#endif
	}

	switch(state)
	{
	case GAMESTATE_WIN:
		{
			struct Font font;
			font_ctor(
				&font, 
				"resources/font.ttf", 
				48, 
				(SDL_Color){255, 255, 255, 255},
				window.renderer
			);

			struct Texture text;
			texture_ctortext(&text, &font, "You Won!", window.renderer);

			SDL_RenderCopy(
				window.renderer, 
				text.raw, 
				NULL, 
				&(SDL_Rect){
					window.width / 2.0 - text.width / 2.0,
					window.height / 2.0 - text.height / 2.0,
					text.width,
					text.height
				}
			);

			window_render(&window);
			SDL_Delay(5000);

			texture_dtor(&text);
			font_dtor(&font);
		}
		break;

	case GAMESTATE_GAMEOVER:
		{
			struct Font font;
			font_ctor(
				&font, 
				"resources/font.ttf", 
				48, 
				(SDL_Color){255, 255, 255, 255},
				window.renderer
			);

			struct Texture text;
			struct Texture image;

			texture_ctortext(&text, &font, "Game Over", window.renderer);
			texture_ctorimage(&image, "resources/grave.png", window.renderer);

			SDL_RenderCopy(
				window.renderer, 
				text.raw, 
				NULL, 
				&(SDL_Rect){
					window.width / 2.0 - text.width / 2.0,
					window.height / 2.0 - 200,
					text.width,
					text.height
				}
			);

			SDL_RenderCopy(
				window.renderer, 
				image.raw, 
				NULL, 
				&(SDL_Rect){
					window.width / 2.0 - image.width * 12 / 2.0,
					window.height / 2.0,
					image.width * 12,
					image.height * 12
				}
			);

			window_render(&window);
			SDL_Delay(5000);

			texture_dtor(&image);
			texture_dtor(&text);
			font_dtor(&font);
		}
		break;

	default:;
	}

	texture_dtor(&gamestate.onscreentexts[0]);
	texture_dtor(&gamestate.onscreentexts[1]);
	texture_dtor(&gamestate.onscreentexts[2]);
	texture_dtor(&gamestate.onscreentexts[3]);
	texture_dtor(&gamestate.onscreentexts[4]);
	texture_dtor(&gamestate.onscreentexts[5]);
	texture_dtor(&gamestate.onscreentexts[6]);
	texture_dtor(&gamestate.timertext);
	font_dtor(&stagefont);
	enemies_dtor(&enemies);
	projectiles_dtor(&projectiles);
	player_dtor(&player);
	map_dtor(&map);
	texture_dtor(&spritesheet);
	inputhandler_dtor(&input);
	window_dtor(&window);
	cleanup();
	//close(clientsocket);
}

