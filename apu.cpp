#include <stdint.h>
#include <iostream>
struct APU
{
    uint8_t sq1[5], sq2[4], wav[5], nse[5];
    uint8_t vol, en, stat;
    uint8_t tab[16];
    uint8_t rd(uint16_t add)
    {
        if (add < 0xff30)
            switch (add & 0xFF)
            {
            case 0x10: //NR1x
                return sq1[0];
            case 0x11:
                return sq1[1];
            case 0x12:
                return sq1[2];
            case 0x13:
                return sq1[3];
            case 0x14:
                return sq1[4];
            case 0x16: //NR2x
                return sq2[0];
            case 0x17:
                return sq2[1];
            case 0x18:
                return sq2[2];
            case 0x19:
                return sq2[3];
            case 0x1A: //NR3x
                return wav[0];
            case 0x1B:
                return wav[1];
            case 0x1C:
                return wav[2];
            case 0x1D:
                return wav[3];
            case 0x1E:
                return wav[4];
            case 0x20: //NR4x
                return nse[0];
            case 0x21:
                return nse[1];
            case 0x22:
                return nse[2];
            case 0x23:
                return nse[3];
            case 0x24: //NR5x
                return vol;
            case 0x25:
                return en;
            case 0x26:
                return stat;
            default:
                //std::cout << "APU Read error:0x" << add << "\n";
                return '-';
            }
        else
            return tab[add & 0xF];
    }
    void wt(uint16_t add, uint8_t data)
    {
        if (add < 0xff30)
            switch (add & 0xFF)
            {
            case 0x10: //NR1x
                sq1[0] = data;
                break;
            case 0x11:
                sq1[1] = data;
                break;
            case 0x12:
                sq1[2] = data;
                break;
            case 0x13:
                sq1[3] = data;
                break;
            case 0x14:
                sq1[4] = data;
                break;
            case 0x16: //NR2x
                sq2[0] = data;
                break;
            case 0x17:
                sq2[1] = data;
                break;
            case 0x18:
                sq2[2] = data;
                break;
            case 0x19:
                sq2[3] = data;
                break;
            case 0x1A: //NR3x
                wav[0] = data;
                break;
            case 0x1B:
                wav[1] = data;
                break;
            case 0x1C:
                wav[2] = data;
                break;
            case 0x1D:
                wav[3] = data;
                break;
            case 0x1E:
                wav[4] = data;
                break;
            case 0x20: //NR4x
                nse[0] = data;
                break;
            case 0x21:
                nse[1] = data;
                break;
            case 0x22:
                nse[2] = data;
                break;
            case 0x23:
                nse[3] = data;
                break;
            case 0x24: //NR5x
                vol = data;
                break;
            case 0x25:
                en = data;
                break;
            case 0x26:
                stat = data;
                break;
            default:
                std::cout << "APU Write error:0x" << add << "\n";
                break;
            }
        else
            tab[add & 0xF] = data;
    }
} apu;