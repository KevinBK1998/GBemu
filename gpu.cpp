#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
const int SCREEN_WIDTH = 160;//*2;
const int SCREEN_HEIGHT = 144;//*2;

struct GPU
{
    uint8_t vram[8192];
    uint8_t oam[160];
    int bgmap, bgtile, scx, scy;
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
            win = SDL_CreateWindow("GBemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
            mode = 2;
            clk = line = 0;
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
    void scanline()
    {
        uint16_t mapoffs = bgmap ? 0x1C00 : 0x1800;
        mapoffs += (((line + scy) & 0xFF) >> 3) << 5; //divide line by 8, multiply by 32 coz 32 tiles in one line
        uint16_t lineoffs = scx >> 3;                 //divide by 8
        uint8_t y = (line + scy) & 0x7;
        uint8_t x = scx & 7;
        uint16_t canvasoffs = line * 160 * 4;
        uint8_t lb;
        uint8_t hb;
        uint16_t tile = vram[mapoffs + lineoffs];
        if (bgtile == 1 && tile < 128)
            tile += 256;
        for (int i = 0; i < 160; i++)
        {
            // 0-blk,1-darkgry,2-lightgray,3-white
            lb = vram[(tile << 4) + (y << 1)];
            hb = vram[(tile << 4) + (y << 1) + 1];
            int v = ((lb >> (7-x)) & 1)*2 + (((hb >> (7-x)) & 1));//doubt lb actuallly lb?
            switch (v)
            {
            case 0:
                SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
                break;
            case 1:
                SDL_SetRenderDrawColor(ren, 192, 192, 192, 0xFF);
                break;
            case 2:
                SDL_SetRenderDrawColor(ren, 96, 96, 96, 0xFF);
                break;
            case 3:
                SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xFF);
                break;
            }
            //SDL_Rect fillRect = { i*2, line*2, i*2+1, line*2+1 };
            SDL_RenderDrawPoint(ren, i, line);//single point
            //SDL_RenderFillRect(ren, &fillRect );
            // When this tile ends, read another
            x++;
            if (x == 8)
            {
                x = 0;
                lineoffs = (lineoffs + 1) & 31;
                tile = vram[mapoffs + lineoffs];
                if (bgtile == 1 && tile < 128)
                    tile += 256;
            }
        }
    }
    void renScreen()
    {
        for (int i = 0; i < 16; i++)
        {
            //for (int j = 0; j < 32; j++)
                std::cout << "0x" << unsigned(vram[ i]) << " ";
            
        }std::cout << std::endl;
        std::cout << "\nTile Map 0:\n";
        int base = 0x1800;
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
                std::cout << "0x" << unsigned(vram[base + i * 32 + j]) << " ";
            std::cout << std::endl;
        }
        for (int t = 0; t < 0x1A; t++)
        {
            std::cout << "\nTile " << t << ":\n";
            for (int i = 0; i < 8; i++)
            {
                int lB = vram[(t << 4) + (i << 1)];
                int hB = vram[(t << 4) + (i << 1) + 1];
                for (int j = 0; j < 8; j++)
                {   int lb = ((lB >> (7-j)) & 1);
                    int hb = ((hB >> (7-j)) & 1);
                    std::cout << hb << lb << ",";
                }
                std::cout << std::endl;
            }
        }
        //std::cin >> base;
        //Event handler
        SDL_Event e;

        //While application is running
        if (quit)
        {
            close();
            exit(1);
        }
        //Handle events on queue
        if (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        SDL_RenderPresent(ren);
    }
} gpu;