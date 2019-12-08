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
    void init(){
        a=b=c=d=e=f=h=l=0;
        sp=pc=0;
    }
    void setbc(uint16_t n){
        c=n&0xFF;
        b=(n&0xFF00)>>8;
    }
    uint16_t getbc(){
        uint16_t r=0;
        r=b&0xFF;
        r<<=8;
        r|=(c&0xFF);
        return r;
    }
    void sethl(uint16_t n){
        l=n&0xFF;
        h=(n&0xFF00)>>8;
    }
    uint16_t gethl(){
        uint16_t r=0;
        r=h&0xFF;
        r<<=8;
        r|=(l&0xFF);
        return r;
    }
    void setsp(uint16_t n){
       sp=n;
    }
};
