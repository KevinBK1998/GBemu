#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
const int SCREEN_WIDTH = 160;  //*2;
const int SCREEN_HEIGHT = 144; //*2;

struct GPU
{
    //GPU registers
    uint8_t ctrl, bgpal, scx, scy, line;
    /*
    LCD&GPU control 0xFF40
                7      |6           |5     |4          |3          |2                         |1      |0
                Display|win:tile map|window|BG tile set|BG tile map|Sprites:size(0-8x8,1-8x16)|Sprites|BG    
    Scroll Y        0xFF42
    Scroll X        0xFF43
    Current scnline 0xFF44
    BG palette      0xFF47
    */
    //GPU memory
    uint8_t vram[8192];
    uint8_t oam[160];
    //flags
    int mode,clk;
    //graphic components
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
            ctrl=0;
            bgpal = 0xE4;
            //Clear screen
            SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(ren);
            SDL_RenderPresent(ren);
        }
    }
    int palMap(int v)
    {
        switch (v)
        {
        case 0:
            return bgpal & 3;
        case 1:
            return (bgpal >> 2) & 3;
        case 2:
            return (bgpal >> 4) & 3;
        case 3:
            return (bgpal >> 6) & 3;
        }
    }
    void scanline()
    {
        uint16_t mapoffs = (ctrl&0x8) ? 0x1C00 : 0x1800;
        mapoffs += (((line + scy) & 0xFF) >> 3) << 5; //divide line by 8, multiply by 32 coz 32 tiles in one line
        uint16_t lineoffs = scx >> 3;                 //divide by 8
        uint8_t y = (line + scy) & 0x7;
        uint8_t x = scx & 7;
        uint16_t canvasoffs = line * 160 * 4;
        uint8_t lb;
        uint8_t hb;
        uint16_t tile = vram[mapoffs + lineoffs];
        if ((ctrl&0x10)  == 1 && tile < 128)
            tile += 256;
        for (int i = 0; i < 160; i++)
        {
            // 0-blk,1-darkgry,2-lightgray,3-white
            lb = vram[(tile << 4) + (y << 1)];
            hb = vram[(tile << 4) + (y << 1) + 1];
            int v = ((lb >> (7 - x)) & 1) * 2 + (((hb >> (7 - x)) & 1)); //doubt lb actuallly lb?
            switch (palMap(v))
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
            //SDL_Rect fillRect = { i*2, line*2, i*2+1, line*2+1 };//scale 200%
            SDL_RenderDrawPoint(ren, i, line); //single point
            //SDL_RenderFillRect(ren, &fillRect );//scale 200% 4px:1px
            // When this tile ends, read another
            x++;
            if (x == 8)
            {
                x = 0;
                lineoffs = (lineoffs + 1) & 31;
                tile = vram[mapoffs + lineoffs];
                if ((ctrl&0x10)  == 1 && tile < 128)
                    tile += 256;
            }
        }
    }
    void renScreen()
    {
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
    uint8_t rd(uint16_t add)
    {
        switch (add & 0xFF)
        {
        case 0x40:
            return ctrl;
        case 0x42:

            return scy;
        case 0x43:
            return scx;
        case 0x44:
            return line;
        case 0x47:
        return bgpal;
        default:
            std::cout << "GPU Read error\n";
            return 0;
        }
    }
    void wt(uint16_t add, uint8_t data)
    {
        switch (add & 0xFF)
        {
        case 0x40:
            ctrl=data;
            break;
        case 0x42:
            scy = data;
            break;
        case 0x43:
            scx = data;
            break;
        case 0x47:
            bgpal = data;
            break;
        default:
            std::cout << "GPU Write error\n";
            break;
        }
    }
    void printState(){
        std::cout << "GPU:\n";
        std::cout << "LCD & GPU Control:" << unsigned(ctrl) << "\n";
        std::cout << "Scroll Y:" << unsigned(scy) << "\n";
        std::cout << "Scroll X:" << unsigned(scx) << "\n";
        std::cout << "Current scanline:" << unsigned(line) << "\n";
        std::cout << "BG palette:" << unsigned(ctrl) << "\n";
    }
} gpu;