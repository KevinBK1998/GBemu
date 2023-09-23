#include <stdint.h>
#include <iostream>
#include <time.h>
const int TIMER_ENABLE = 0b100;
struct TIMER
{
    uint8_t divReg, countReg, modReg, ctrlReg;
    int clockTrigger, divPrevTime, prevTime, rtcFlags;
    bool int50;
    time_t latchedTime;

    void timePassed(int time){
        for (int i = 0; i < time; i++)
        {
            if ((divPrevTime + i) % 256 == 0)
                divReg++;
            if (ctrlReg & TIMER_ENABLE) {
                if ((prevTime + i) % clockTrigger == 0)
                    if (countReg + 1u > 0xFFu) {
                        int50 = true;
                        countReg = modReg;
                    } else
                        countReg++;
            }
        }
        divPrevTime += time;
        if (ctrlReg & TIMER_ENABLE)
            prevTime += time;
    }

    bool hasOverflowOccurred(){
        if (int50) {
            int50 = false;
            return true;
        }
        return false;
    }
    void latchClockData() {
        // TODO: 24/8/20 When Day Counter overflows Carry flag should be set
        latchedTime = time(NULL);
    }

    uint8_t read(uint16_t address) {
        switch (address)
        {
            case 0xFF04: return divReg;
            case 0xFF05: return countReg;
            case 0xFF06: return modReg;
            default: return ctrlReg;
        }
    }

    void write(uint16_t address, uint8_t data) {
        switch (address & 0xF)
        {
        case 4:
            cout<<"TMR:DIV reset";
            divReg = 0;
            divPrevTime = 1;
            break;
        case 5:
            cout<<"TMR:Counter write "<< data;
            countReg = data;
            break;
        case 6:
            if (data > 0)
                cout<<"TMR:Modulo write "<< data;
            modReg = data;
            break;
        case 7:
            if (data > 0)
                cout<<"TMR:Control write "<< data;
            ctrlReg = data & 7;
            switch (data&3)
            {
            case 1:
                clockTrigger = 16;
                break;
            case 2:
                clockTrigger = 64;
                break;
            case 3:
                clockTrigger = 256;
                break;
            default:
                clockTrigger = 1024;
                break;
            }
            if (data & TIMER_ENABLE)
                prevTime = 1;
            break;
        default:
            cout<<"TMR:Invalid write "<< data <<" to "<< address;
        }
    }
    void printState(){
        cout << "\nTIMER:\n";
        cout << "divReg:" << unsigned(divReg) << "\tctrlReg:" << unsigned(ctrlReg) << "\tcountReg:" << unsigned(countReg) << "\tmodReg:" << unsigned(modReg) << "\n";
    }
} timer;
