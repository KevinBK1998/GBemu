#ifndef REG_h
#define REG_h
#include "registers.h"
#endif
void registers::reset()
{
    a = b = c = d = e = f = h = l = 0;
    sp = pc = 0;
}
void registers::setbc(uint16_t n)
{
    c = n & 0xFF;
    b = (n & 0xFF00) >> 8;
}
uint16_t registers::getbc()
{
    uint16_t r = 0;
    r = b & 0xFF;
    r <<= 8;
    r |= (c & 0xFF);
    return r;
}
void registers::setde(uint16_t n)
{
    e = n & 0xFF;
    d = (n & 0xFF00) >> 8;
}
uint16_t registers::getde()
{
    uint16_t r = 0;
    r = d & 0xFF;
    r <<= 8;
    r |= (e & 0xFF);
    return r;
}
void registers::sethl(uint16_t n)
{
    l = n & 0xFF;
    h = (n & 0xFF00) >> 8;
}
uint16_t registers::gethl()
{
    uint16_t r = 0;
    r = h & 0xFF;
    r <<= 8;
    r |= (l & 0xFF);
    return r;
}
