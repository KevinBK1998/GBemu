#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <stdint.h>
#include "gpu.cpp"
#include "apu.cpp"
#include "timer.cpp"
using namespace std;
struct MMU
{
    int b;
    const uint8_t bios[256] = {
        0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
        0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
        0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
        0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
        0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
        0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
        0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
        0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
        0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
        0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
        0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
        0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
        0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
    };
    uint8_t rom[2097152]; //2MB
    uint8_t wram[8192];
    uint8_t eram[32768]; //32KB
    uint8_t zram[128];
    uint8_t ie, ifl, cartType, eExRam, romBk, ramBk, mde;
    uint romOffset, ramOffset;
    void reset()
    {
        b = 1;
        ie = 0;
        ifl = 0;
        ramOffset = 0x0000;
        romOffset = 0x4000;
    }
    void load(char *name)
    {
        char c;
        ifstream fin(name);
        if (!fin)
        {
            cout << "Error Opening ROM\n";
            exit(-1);
        }
        int i = 0;
        while (i < 2097152 && fin)
        {
            fin.get(c);
            rom[i++] = c;
        }
        cartType = rom[0x147];
        char nme[16];
        int ind;
        for (ind = 0; rom[0x134 + ind] != 0; ind++)
            nme[ind] = rom[0x134 + ind];
        nme[ind] = '\0';
        cout << "Loaded:" << nme << endl;
    }
    uint8_t read8(uint16_t add)
    {
        switch (add & 0xF000)
        {
        case 0x0000:
            if (b)
            {
                if (add < 0x0100)
                    return bios[add];
            }
            return rom[add];
        case 0x1000:
        case 0x2000:
        case 0x3000:
            return rom[add];
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            return rom[romOffset + (add & 0x3FFF)];
        case 0x8000:
        case 0x9000:
            return gpu.vram[add & 0x1FFF];
        case 0xA000:
        case 0xB000:
            return eram[ramOffset + (add & 0x1FFF)];
        case 0xC000:
        case 0xD000:
            return wram[add & 0x1FFF];
        case 0xE000:
            return wram[add & 0x1FFF];
        case 0xF000:
            switch (add & 0x0F00)
            {
            case 0xE00:
                if (add < 0xFEA0)
                    return gpu.oam[add & 0xFF];
                else
                    return '-';
            case 0xF00:
                if (add == 0xFF50)
                    return !b;
                else if (add == 0xFFFF)
                    return ie;
                else if (add < 0xFF80) //Memory MAP
                    switch (add & 0xF0)
                    {
                    case 0x00:
                        if (add == 0xFF00)
                            return joyp.read();
                        else if (add > 0xFF02 && add < 0xFF08)
                            return timer.read(add);
                        else if (add == 0xFF0F)
                            return ifl;
                        else
                            //cout << "Unable to read 0x" << add << endl;
                            return '-';
                    case 0x10: //Sound reg
                    case 0x20:
                    case 0x30:
                        return apu.rd(add);
                    case 0x40:
                    case 0x50:
                    case 0x60:
                    case 0x70: //GPU reg
                        return gpu.rd(add);
                    default:
                        //cout << "Unable to read 0x" << add << endl;
                        return '-';
                    }

                else
                    return zram[add & 0x7F];
            default:
                return wram[add & 0x1FFF];
            }
        }
    }
    uint16_t read16(uint16_t add)
    {
        return (read8(add + 1) << 8) + read8(add);
    }
    void write8(uint16_t add, uint8_t data)
    {
        switch (add & 0xF000)
        {
        case 0x0000:
        case 0x1000:
            if (cartType == 2 || cartType == 3)
            { //request external ram
                if ((data & 0xF) == 0x0A)
                    eExRam = 1;
                else
                    eExRam = 0;
            }
            break;
        case 0x2000:
        case 0x3000:
            if (cartType == 1 || cartType == 2 || cartType == 3)
            { //rom bank set lower 5 bits
                data &= 0x1F;
                if (!data)
                    data = 1;
                romBk &= 0x60;
                romBk |= data;
                romOffset = romBk * 0x4000;
            }
            break;
        case 0x4000:
        case 0x5000:
            if (cartType == 1 || cartType == 2 || cartType == 3)
            {
                if (mde)
                { //ram bank
                    ramBk |= (data & 0x3);
                    ramOffset = ramBk * 0x2000;
                }
                else
                { //rom bank set upper 2 bits
                    data &= 0x3;
                    data <<= 5;
                    romBk &= 0x1F;
                    romBk |= data;
                    romOffset = romBk * 0x4000;
                }
            }
            break;
        case 0x6000:
        case 0x7000:
            if (cartType == 2 || cartType == 3) //mode switch
                mde = data & 1;
            break;
        case 0x8000:
        case 0x9000:
            gpu.vram[add & 0x1FFF] = data;
            break;
        case 0xA000:
        case 0xB000:
            eram[ramOffset + add & 0x1FFF] = data;
            break;
        case 0xC000:
        case 0xD000:
        case 0xE000:
            wram[add & 0x1FFF] = data;
            break;
        case 0xF000:
            switch (add & 0x0F00)
            {
            case 0xE00:
                gpu.oam[add & 0xFF] = data;
                gpu.updateObj(add & 0xFF, data);
                break;
            case 0xF00:
                if (add == 0xFF50)
                    b = !data;
                else if (add == 0xFFFF)
                    ie = data;
                else if (add == 0xFF46)
                    dmaTransfer(data);
                else if (add < 0xFF80) //Memory MAP
                    switch (add & 0xF0)
                    {
                    case 0x00:
                        if (add == 0xFF00)
                            joyp.write(data);
                        else if (add > 0xFF02 && add < 0xFF08)
                            timer.write(add, data);
                        else if (add == 0xFF0F)
                            ifl = data;
                        else
                            cout << "Unable to write 0x" << add << endl;
                        break;
                    case 0x10: //Sound reg
                    case 0x20:
                    case 0x30:
                        apu.wt(add, data);
                        break;
                    case 0x40:
                    case 0x50:
                    case 0x60:
                    case 0x70: //GPU reg
                        gpu.wt(add, data);
                        break;
                    default:
                        cout << "Unable to write 0x" << add << endl;
                        break;
                    }
                else
                    zram[add & 0x7F] = data;
                break;
            default:
                wram[add & 0x1FFF] = data;
            }
            break;
        }
    }
    void write16(uint16_t add, uint16_t data)
    {
        write8(add, data & 0xFF);
        write8(add + 1, ((data & 0xFF00) >> 8));
        return;
    }
    void dump()
    {
        ofstream fout("memory.txt");
        if (!fout)
        {
            cout << "Error Opening File\n";
            exit(-1);
        }
        int hB = 0;
        fout << hex << uppercase;
        while (hB <= 0xFF)
        {
            fout << hB << ":\n";
            for (int hb = 0; hb <= 0xF; hb++)
            {
                for (int lb = 0; lb <= 0xF; lb++)
                {
                    uint8_t v = read8((hB << 8) + (hb << 4) + lb);
                    if (v == '-')
                        fout << v << " ";
                    else
                        fout << unsigned(v) << " ";
                }
                fout << endl;
            }
            hB++;
        }
    }
    void dumpset()
    {
        ofstream fout("tileset.bin", ios::out|ios::binary);
        if (!fout)
        {
            cout << "Error Opening File\n";
            exit(-1);
        }
        uint16_t i = 0x8000;
        while (i < 0x9800)
        {
            uint8_t v = read8(i);
            fout.write((char*) &v, sizeof(uint8_t));
            i++;
        }
    }
    void dumpmap()
    {
        ofstream textOut("bgmap.txt");
        ofstream fout("bgmap.bin", ios::out|ios::binary);
        if (!(fout && textOut))
        {
            cout << "Error Opening Dump File\n";
            exit(-1);
        }
        int hB = 0x98;
        while (hB < 0xA0)
        {
            textOut << hex << uppercase << hB << "00:\n";
            for (int hb = 0; hb <= 0xF; hb += 2)
            {
                for (int lb = 0; lb < 0x20; lb++)
                {
                    uint8_t v = read8((hB << 8) + (hb << 4) + lb);
                    if ((v > 0x1F && v <= 0x7F))
                        textOut << v << "  ";
                    else{
                        textOut << unsigned(v)<< "T";
                        if (v < 16)
                        textOut << " ";
                    }
                    fout.write((char*) &v, sizeof(uint8_t));
                }
                textOut << endl;
            }
            hB++;
        }
    }

    void printState()
    {
        cout << "\nMMU:\n";
        cout << "inBIOS:" << b << "\tcartType:" << unsigned(cartType) << "\tERAM:" << unsigned(eExRam) << "\tMODE:" << unsigned(mde) << endl;
        cout << "ROMbase:" << romOffset << "\tRAMbase:" << ramOffset << "\tROMBank:" << unsigned(romBk) << "\tRAMBank:" << unsigned(ramBk) << endl;
        cout << "IE:" << unsigned(ie) << "\tIF:" << unsigned(ifl) << endl;
    }
    void dmaTransfer(uint8_t data)
    {
        uint16_t base1 = (data << 8) & 0x1FFF;
        if (data > 0xDF || data < 0x80)
            return;
        for (int i = 0; i < 0xA0; i++)
            gpu.oam[i] = wram[base1 + i];
        cout << "DMA Transfer Complete" << endl;
    }
} mmu;