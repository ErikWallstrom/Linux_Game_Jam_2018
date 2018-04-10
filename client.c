#include "API2/inputhandler.h"
#include "API2/initialize.h"
#include "API2/transition.h"
#include "API2/texture.h"
#include "API2/window.h"
#include "API2/font.h"
#include "API2/rect.h"
#include "API2/log.h"
#include "player.h"
#include "map.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define TICKS_PER_SEC 30.0
#define TICK_RATE (1000.0 / TICKS_PER_SEC)

struct GameState
{
	struct Window* window;
	struct InputHandler* input;
	struct Texture* spritesheet;
	struct Map* map;
	struct Player* player;
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
		case SDL_KEYUP:
			switch(gamestate.input->events[i].key.keysym.sym)
			{
			case SDLK_w:
				gamestate.player->selectedanimation = PLAYERSTANDING_BACK;
				break;
			case SDLK_s:
				gamestate.player->selectedanimation = PLAYERSTANDING_FRONT;
				break;
			case SDLK_a:
				gamestate.player->selectedanimation = PLAYERSTANDING_LEFT;
				break;
			case SDLK_d:
				gamestate.player->selectedanimation = PLAYERSTANDING_RIGHT;
				break;
			default:;
			}
			break;
		case SDL_QUIT:
			*gamestate.running = 0;
		default:;
		}
	}

	double speedmod = 1.0;
	gamestate.player->oldpos = gamestate.player->rect.pos;
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
		gamestate.player->rect.pos.x -= PLAYER_SPEED * speedmod;
		gamestate.player->selectedanimation = PLAYERWALKING_LEFT;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_D]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTX) > 0)
	{
		gamestate.player->rect.pos.x += PLAYER_SPEED * speedmod;
		gamestate.player->selectedanimation = PLAYERWALKING_RIGHT;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_W]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_UP)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTY) < 0)
	{
		gamestate.player->rect.pos.y -= PLAYER_SPEED * speedmod;
		gamestate.player->selectedanimation = PLAYERWALKING_UP;
	}
	if(gamestate.input->keystate[SDL_SCANCODE_S]
		|| SDL_GameControllerGetButton(
			gamestate.input->controller,
			SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		|| SDL_GameControllerGetAxis(
			gamestate.input->controller,
			SDL_CONTROLLER_AXIS_LEFTY) > 0)
	{
		gamestate.player->rect.pos.y += PLAYER_SPEED * speedmod;
		gamestate.player->selectedanimation = PLAYERWALKING_DOWN;
	}

	player_update(gamestate.player);
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

	map_render(gamestate.map, camerax, cameray);
	player_render(gamestate.player, interpolation, delta, camerax, cameray);
	SDL_SetRenderDrawColor(gamestate.window->renderer, 0, 0, 0, 255);
}

int main(int argc, char* argv[])
{
	log_seterrorhandler(onerror, NULL);
	/*
	if(argc < 3)
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
	}
	*/

	initialize();

	struct Window window;
	window_ctor(&window, "Game Window", 1600, 900, WINDOW_DEFAULT);

	struct InputHandler input;
	inputhandler_ctor(&input);

	struct Texture spritesheet;
	texture_ctorimage(&spritesheet, "resources/tiles.png", window.renderer);

	struct Map map;
	map_ctor(&map, window.renderer);

	struct Player player;
	player_ctor(&player, window.renderer);

	int running = 1;
	gamestate.running = &running;
	gamestate.spritesheet = &spritesheet;
	gamestate.window = &window;
	gamestate.input = &input;
	gamestate.map = &map;
	gamestate.player = &player;

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
	}

	player_dtor(&player);
	map_dtor(&map);
	texture_dtor(&spritesheet);
	inputhandler_dtor(&input);
	window_dtor(&window);
	cleanup();
	//close(clientsocket);
}

