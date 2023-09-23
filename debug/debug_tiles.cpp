#include <SDL2/SDL.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
using namespace std;
const int SCREEN_WIDTH_TILES = 16;
const int SCREEN_WIDTH = 9*SCREEN_WIDTH_TILES;
const int SCREEN_HEIGHT = 9*SCREEN_WIDTH_TILES;
//The window we'll be rendering to
SDL_Window *gWindow = NULL;
//The window renderer
SDL_Renderer *gRenderer = NULL;
//Palette
uint8_t pal = 0xfc;

//palette remap
void remap(int v)
{
	switch ((pal >> (v * 2)) & 3)
	{
	case 0:
		//cout << "0";
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		break;
	case 1:
		//cout << "1";
		SDL_SetRenderDrawColor(gRenderer, 0xC0, 0xC0, 0xC0, 0xFF);
		break;
	case 2:
		//cout << "2";
		SDL_SetRenderDrawColor(gRenderer, 0x60, 0x60, 0x60, 0xFF);
		break;
	case 3:
		//cout << "3";
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		break;
	}
}
//Starts up SDL and creates window
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Tile Map", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}
//Frees media and shuts down SDL
void close()
{
	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char *args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		char* name = "tileset.bin";
		if (argc>1)
			name=args[argc - 1];
		ifstream fin(name);
		if (!fin)
		{
			cout << "Error Opening file\n";
			exit(-1);
		}
		//Main loop flag
		bool quit = false;
		//Event handler
		SDL_Event e;

		//Tile data
		uint8_t tile[0x1800];
		char c;
		for (int i = 0; i < 0x1800; i++)
		{
			fin.get(c);
			tile[i] = c;
		}
		fin.close();
		
		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			//Clear screen
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_RenderClear(gRenderer);
			//Draw
			for(int tno=0;tno<0x180;tno++){
				for (int y = 0; y < 8; y++)
				{
					uint8_t hb = tile[tno*16 + y*2 + 1];
					uint8_t lb = tile[tno*16 + y*2];
					for (int x = 0; x < 8; x++)
					{
						int strength = ((lb >> (7 - x)) & 1) + 2 * ((hb >> (7 - x)) & 1);
						remap(strength);
						//SDL_Rect fillRect = {2 * x, 2 * y, 2 * x + 1, 2 * y + 1};
						//SDL_RenderFillRect(gRenderer, &fillRect);
						uint8_t xoff=(tno%SCREEN_WIDTH_TILES)*9,yoff=(tno/SCREEN_WIDTH_TILES)*9;
						SDL_RenderDrawPoint(gRenderer, xoff+x, yoff+y);
					}
				}
			}
			//Update screen
			SDL_RenderPresent(gRenderer);
			// cin >> c;
			// if(c=='b'){
			// 	quit = true;
			// 	break;
			// }
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}