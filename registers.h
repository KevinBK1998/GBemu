#include<stdint.h>
struct registers{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;  //z:f&0x80,n:f&0x40,h:f&0x20,c:f&0x10
    uint8_t h;
    uint8_t l;
    uint16_t pc;
    uint16_t sp;
    void reset();
    void setbc(uint16_t n);
    uint16_t getbc();
    void setde(uint16_t n);
    uint16_t getde();
    void sethl(uint16_t n);
    uint16_t gethl();
};
