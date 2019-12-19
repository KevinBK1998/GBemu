#include <SDL2/SDL.h>
#include <iostream>
using namespace std;
struct keys
{
    uint8_t row[2];
    uint8_t col;
    void reset(){
        row[0]=0xF;
        row[1]=0xF;
        col=0;
    }
    uint8_t read()
    {
        switch (col)
        {
        case 0x10:
            return row[0];
            break;
        case 0x20:
            return row[1];
            break;
        default:
            return 0;
            break;
        }
    }
    void write(uint8_t data)
    {
        col = data & 0x30;
    }
    void input(SDL_Event &e)
    {
        if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_RETURN:
                kdown('S');
                break;
            case SDLK_SPACE:
                kdown('s');
                break;
            case SDLK_z:
                kdown('a');
                break;
            case SDLK_x:
                kdown('b');
                break;
            case SDLK_UP:
                kdown('u');
                break;
            case SDLK_DOWN:
                kdown('d');
                break;
            case SDLK_RIGHT:
                kdown('r');
                break;
            case SDLK_LEFT:
                kdown('l');
                break;
            }
        }
        else if (e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_RETURN:
                kup('S');
                break;
            case SDLK_SPACE:
                kup('s');
                break;
            case SDLK_z:
                kup('a');
                break;
            case SDLK_x:
                kup('b');
                break;
            case SDLK_UP:
                kup('u');
                break;
            case SDLK_DOWN:
                kup('d');
                break;
            case SDLK_RIGHT:
                kup('r');
                break;
            case SDLK_LEFT:
                kup('l');
                break;
            }
        }
    }
    void kdown(char in)
    {
        switch (in)
        {
        case 'a':
            row[0]&=0xE;
            break;
        case 'b':
            row[0]&=0xD;
            break;
        case 's':
            row[0]&=0xB;
            break;
        case 'S':
            row[0]&=0x7;
            break;
        case 'r':
            row[1]&=0xE;
            break;
        case 'l':
            row[1]&=0xD;
            break;
        case 'u':
            row[1]&=0xB;
            break;
        case 'd':
            row[1]&=0x7;
            break;
        }
    }
    void kup(char in)
    {
        switch (in)
        {
        case 'a':
            row[0]|=0x1;
            break;
        case 'b':
            row[0]|=0x2;
            break;
        case 's':
            row[0]|=0x4;
            break;
        case 'S':
            row[0]|=0x8;
            break;
        case 'r':
            row[1]|=0x1;
            break;
        case 'l':
            row[1]|=0x2;
            break;
        case 'u':
            row[1]|=0x4;
            break;
        case 'd':
            row[1]|=0x8;
            break;
        }
    }
} joyp;