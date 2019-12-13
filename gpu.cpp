#include <stdint.h>
#include <SDL2/SDL.h>
const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;

struct GPU
{
    uint8_t vram[8192];
    uint8_t oam[160];
    int mode;
    int line;
    int clk;
    SDL_Window *win;
    SDL_Renderer *ren;
    bool quit;
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
            win = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            if (win == NULL)
            {
                printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Create renderer for window
                ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
                if (ren == NULL)
                {
                    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
                else
                {
                    //Initialize renderer color
                    SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
                }
            }
        }

        return success;
    }
    //Frees media and shuts down SDL
    void close()
    {
        //Destroy window
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        win = NULL;
        ren = NULL;

        //Quit SDL subsystems
        SDL_Quit();
    }
    void reset()
    {
        //Start up SDL and create window
        if (!init())
        {
            printf("Failed to initialize!\n");
        }
        else
        {
            //Main loop flag
            quit = false;
            //Clear screen
            SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(ren);
            SDL_RenderPresent(ren);
        }
    }
    void loop()
    {
        //Event handler
        SDL_Event e;

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
            SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(ren);
            SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0x00, 0xFF);
            for (int i = 0; i < SCREEN_WIDTH; i++)
                for (int j = 0; j < SCREEN_HEIGHT; j++)
                    SDL_RenderDrawPoint(ren, i, j);
            //Update screen
            SDL_RenderPresent(ren);
        }
    }
} gpu;