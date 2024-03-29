#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include "keys.cpp"
using namespace std;
const int SCREEN_WIDTH = 160;  //*2;
const int SCREEN_HEIGHT = 144; //*2;
enum MODE{
    HBLANK,
    VBLANK,
    OAM,
    VRAM,
};
typedef struct OBJM
{
    uint8_t y, x, tile;
    struct OPT
    {
        bool pty, yf, xf, palNo;
        void byte2flags(uint8_t data)
        {
            pty = (data > 0x7F);
            yf = ((data & 0x40) != 0);
            xf = ((data & 0x20) != 0);
            palNo = ((data & 0x10) != 0);
        }
    } opt;
    void update(uint8_t type, uint8_t data)
    {
        switch (type)
        {
        case 0:
            y = data - 16;
            break;
        case 1:
            x = data - 8;
            break;
        case 2:
            tile = data;
            break;
        case 3:
            opt.byte2flags(data);
            break;
        }
    }
} OBJM;
struct GPU
{
    //GPU registers
    uint8_t bgpal, scx, scy, line, linec, dma, wy, wx, objpal[2];
    struct LCDC
    {
        bool lcdOn, winMap, winOn, bgSet, bgMap, sprSize, sprOn, bgOn;
        uint8_t flags2byte()
        {
            return (lcdOn * 0x80 + winMap * 0x40 + winOn * 0x20 + bgSet * 0x10 + bgMap * 8 + sprSize * 4 + sprOn * 2 + bgOn);
        }
        void byte2flags(uint8_t data)
        {
            lcdOn = (data > 0x7F);
            winMap = ((data & 0x40) != 0);
            winOn = ((data & 0x20) != 0);
            bgSet = ((data & 0x10) != 0);
            bgMap = ((data & 8) != 0);
            sprSize = ((data & 4) != 0);
            sprOn = ((data & 2) != 0);
            bgOn = (data & 1);
        }
    } ctrl;
    struct STAT
    {
        uint8_t intSelect, match, mode;
        uint8_t flags2byte()
        {
            return (intSelect << 3 + match << 2 + mode);
        }
        void byte2flags(uint8_t data)
        {
            intSelect = (data & 0x78);
            match = ((data & 4) != 0);
        }
    } stat;
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
    uint8_t oam[256];
    //object meta
    OBJM objAttr[40];
    uint8_t row[160];
    //flags
    int clk;
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
            cout << "SDL could not initialize! SDL Error: "<<SDL_GetError()<<endl;
            success = false;
        }
        else
        {
            //Set texture filtering to linear
            if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            {
                cout<<"Warning: Linear texture filtering not enabled!";
            }

            //Create window
            win = SDL_CreateWindow("GBemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            if (win == NULL)
            {
                cout<<"Window could not be created! SDL Error: "<< SDL_GetError()<<endl;
                success = false;
            }
            else
            {
                //Create renderer for window
                ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
                if (ren == NULL)
                {
                    cout<<"Renderer could not be created! SDL Error: "<< SDL_GetError()<<endl;
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
            cout<<"Failed to initialize!"<<endl;
        }
        else
        {
            //Main loop flag
            quit = false;
            stat.mode = 2;
            clk = line = 0;
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
    int oPalMap(int v, int pal)
    {
        switch (v)
        {
        case 0:
            return objpal[pal] & 3;
        case 1:
            return (objpal[pal] >> 2) & 3;
        case 2:
            return (objpal[pal] >> 4) & 3;
        case 3:
            return (objpal[pal] >> 6) & 3;
        }
    }
    void bgScan()
    {
        uint16_t mapoffs = ctrl.bgMap ? 0x1C00 : 0x1800;
        mapoffs += (((line + scy) & 0xFF) >> 3) << 5; //divide line by 8, multiply by 32 coz 32 tiles in one line
        uint16_t lineoffs = scx >> 3;                 //divide by 8
        uint8_t y = (line + scy) & 0x7;
        uint8_t x = scx & 7;
        for (int i = 0; i < 160;)
        {
            uint16_t tile = vram[mapoffs + lineoffs];
            if (!ctrl.bgSet && tile < 128)
                tile += 256;
            uint8_t lb = vram[(tile << 4) + (y << 1)];
            uint8_t hb = vram[(tile << 4) + (y << 1) + 1];
            for (; x < 8; x++, i++)
            {
                row[i] = ((lb >> (7 - x)) & 1) + 2 * (((hb >> (7 - x)) & 1));
                switch (palMap(row[i]))
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
                SDL_RenderDrawPoint(ren, i, line);
            }
            x = 0;
            lineoffs = (lineoffs + 1) & 31;
        }
    }
    void spriteScan()
    {
        uint8_t lb, hb, tlrw[8];
        for (int i = 0; i < 40; i++)
        {
            uint8_t y = objAttr[i].y;
            if (y <= line && (y + 8) > line)
            {
                int n = objAttr[i].opt.palNo;
                uint8_t tile = objAttr[i].tile;
                if (objAttr[i].opt.yf)
                {
                    lb = vram[(tile << 4) + ((7 - (line - y)) << 1)];
                    hb = vram[(tile << 4) + ((7 - (line - y)) << 1) + 1];
                }
                else
                {
                    lb = vram[(tile << 4) + ((line - y) << 1)];
                    hb = vram[(tile << 4) + ((line - y) << 1) + 1];
                }
                // if (tile)
                // {
                //     cout << "tile:" << unsigned(tile) << " row:" << unsigned(line - y) << " hb:" << unsigned(hb) << " lb:" << unsigned(lb) << endl;
                //     cin >> n;
                // }
                for (int x = 0; x < 8; x++)
                    if (objAttr[i].opt.xf)
                        tlrw[x] = ((lb >> x) & 1) + 2 * (((hb >> x) & 1));
                    else
                        tlrw[x] = ((lb >> (7 - x)) & 1) + 2 * (((hb >> (7 - x)) & 1));
                // if (tile)
                // {
                //     cout << "Line:" << unsigned(line) << endl;
                //     for (int x = 0; x < 8; x++)
                //         cout << unsigned(tlrw[x]) << " ";
                //     cout << endl;
                //     cin >> n;
                // }
                for (int x = 0; x < 8; x++)
                {
                    uint8_t xco = objAttr[i].x;
                    if (((xco + x) >= 0 && (xco + x < 160)) && (tlrw[x]) && (objAttr[i].opt.pty || !row[xco + x]))
                    {
                        switch (oPalMap(tlrw[x], n))
                        {
                        case 0:
                            SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
                            break;
                        case 1:
                            SDL_SetRenderDrawColor(ren, 0xC0, 0xC0, 0xC0, 0xFF);
                            break;
                        case 2:
                            SDL_SetRenderDrawColor(ren, 0x60, 0x60, 0x60, 0xFF);
                            break;
                        case 3:
                            SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
                            break;
                        }
                        SDL_RenderDrawPoint(ren, xco + x, line);
                    }
                }
            }
        }
    }
    void scanline()
    {
        if (ctrl.bgOn)
            bgScan();
        if (ctrl.sprOn)
            spriteScan();
    }
    int renScreen()
    {
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
            int i=joyp.input(e);
            if (i)
                return i;
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        SDL_RenderPresent(ren);
        return 0;
    }
    uint8_t rd(uint16_t add)
    {
        switch (add & 0xFF)
        {
        case 0x40:
            return ctrl.flags2byte();
        case 0x41:
            return stat.flags2byte();
        case 0x42:
            return scy;
        case 0x43:
            return scx;
        case 0x44:
            return line;
        case 0x45:
            return linec;
        case 0x4A:
            return wy;
        case 0x4B:
            return wx;
        default:
            //cout << "GPU Read error:0x" << add << "\n";
            return '-';
        }
    }
    void wt(uint16_t add, uint8_t data)
    {
        switch (add & 0xFF)
        {
        case 0x40:
            if (ctrl.lcdOn)
            {
                ctrl.byte2flags(data);
                if (!ctrl.lcdOn)
                    line = 0;
            }
            else
                ctrl.byte2flags(data);
            break;
        case 0x41:
            stat.byte2flags(data);
            break;
        case 0x42:
            scy = data;
            break;
        case 0x43:
            scx = data;
            break;
        case 0x45:
            linec = data;
            break;
        case 0x47:
            bgpal = data;
            break;
        case 0x48:
            objpal[0] = data;
            break;
        case 0x49:
            objpal[1] = data;
            break;
        case 0x4A:
            wy = data;
            break;
        case 0x4B:
            wx = data;
            break;
        default:
            cout << "GPU Write error:0x" << add << "\n";
            break;
        }
    }

    char* getModeString(){
        switch (stat.mode)
        {
        case HBLANK: return "HBLANK";
        case VBLANK: return "VBLANK";
        case OAM: return "OAM";
        case VRAM: return "VRAM";
        default: return "UNKNOWN";
        }
    }

    void printState()
    {
        cout << "\nGPU:\n";
        cout << "LCD & GPU Control:" << unsigned(ctrl.flags2byte()) << "\tStatus:" << unsigned(stat.flags2byte()) << "\tMODE:" << getModeString() << "\n";
        cout << "Scroll Y:" << unsigned(scy) << "\tScroll X:" << unsigned(scx) << "\n";
        cout << "Current scanline:" << unsigned(line) << "\n";
        cout << "BG palette:" << unsigned(bgpal) << "\tOBJ0 palette:" << unsigned(objpal[0]) << "\tOBJ1 palette:" << unsigned(objpal[1]) << "\n";
    }
    void updateObj(uint8_t add, uint8_t data)
    {
        int objno = add >> 2;
        if (objno < 40)
            objAttr[objno].update(add & 3, data);
    }
    void clear()
    {
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(ren);
    }
    void dumpoam()
    {
        ofstream fout("oam.txt");
        if (!fout)
        {
            cout << "Error Opening File\n";
            exit(-1);
        }
        int i = 0;
        fout << hex << uppercase;
        while (i < 40)
        {
            fout << i << ":\n";
            for (int j = 0; j < 4; j++)
            {

                uint8_t v = oam[i * 4 + j];
                fout << unsigned(v) << " ";
            }
            i++;
            fout << endl;
        }
    }
} gpu;