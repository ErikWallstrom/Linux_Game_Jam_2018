#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "log.h"

void initialize(void)
{
	SDL_version compileversion;
	SDL_version linkversion;

	SDL_VERSION(&compileversion);
	SDL_GetVersion(&linkversion);
	if(compileversion.major != linkversion.major 
		|| compileversion.minor != linkversion.minor 
		|| compileversion.patch != linkversion.patch)
	{
		log_warning(
			"Program was compiled with SDL version %i.%i.%i, but was linked"
				" with version %i.%i.%i\n",
			compileversion.major,
			compileversion.minor,
			compileversion.patch,
			linkversion.major,
			linkversion.minor,
			linkversion.patch
		);
	}

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)) //Is this enough?
	{
		log_error("%s", SDL_GetError());
	}

	const SDL_version* imglinkversion = IMG_Linked_Version();
	SDL_IMAGE_VERSION(&compileversion);
	if(compileversion.major != imglinkversion->major 
		|| compileversion.minor != imglinkversion->minor 
		|| compileversion.patch != imglinkversion->patch)
	{
		log_warning(
			"Program was compiled with SDL_image version %i.%i.%i, but was "
				" linked with version %i.%i.%i\n",
			compileversion.major,
			compileversion.minor,
			compileversion.patch,
			imglinkversion->major,
			imglinkversion->minor,
			imglinkversion->patch
		);
	}

	if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
	{
		log_error("%s", IMG_GetError());
	}

	const SDL_version* ttflinkversion = TTF_Linked_Version();
	SDL_TTF_VERSION(&compileversion);
	if(compileversion.major != ttflinkversion->major 
		|| compileversion.minor != ttflinkversion->minor 
		|| compileversion.patch != ttflinkversion->patch)
	{
		log_warning(
			"Program was compiled with SDL_ttf version %i.%i.%i, but was linked"
				" with version %i.%i.%i\n",
			compileversion.major,
			compileversion.minor,
			compileversion.patch,
			ttflinkversion->major,
			ttflinkversion->minor,
			ttflinkversion->patch
		);
	}

	if(TTF_Init())
	{
		log_error("%s", TTF_GetError());
	}

	srand(time(NULL));
}

void cleanup(void)
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

