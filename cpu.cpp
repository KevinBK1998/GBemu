#include "registers.cpp"
#include<iostream>
using namespace std;
struct CPU{
    registers reg;
    //0x Instructions
    void nop(){
        cout<<"NOP\n";
    }
    void ldbc(uint16_t n){
        reg.setbc(n);
    }
    void ldBC_a(){
        //LD [BC],A
        //TODO everything
        return;
    }
    void inc_bc(){
        int res=reg.getbc()+1;
        cout<<"bc:"<<reg.getbc()<<"\n";
        cout<<"res:"<<res<<"\n";
        reg.setbc(reg.getbc()+1);
    }
    void inc_b(){
        uint8_t result=reg.b+1;
        reg.f&=0x10;
        if(!result)
            reg.f|=0x80;
        if((reg.b&0xF)+1>0xF)
            reg.f|=0x20;
        reg.b=result;
    }
    void dec_b(){
        uint8_t result=reg.b-1;
        reg.f&=0x10;
        if(!result)
            reg.f|=0x80;
        reg.f|=0x40;
        if((reg.b&0xF)-1<0)
            reg.f|=0x20;
        reg.b=result;
    }
    void ldb_n(uint8_t n){
        reg.b=n;
    }
    void rlc_a(){
        int c=((reg.f&0x10)!=0);
        reg.f=0;
        reg.f|=((reg.a&0x80)>>3);
        reg.a<<=1;
        reg.a+=c;
        if(!reg.a)
            reg.f|=0x80;
    }
    void ldM_sp(uint16_t m){
        //LD [nnnn],SP
        //TDDO everything
        return;
    }
    void addhl_bc(){
        int result=reg.gethl()+reg.getbc();
        reg.f&=0x80;
        if((reg.gethl()&0xFFF)+(reg.getbc()&0xFFF)>0xFFF)
            reg.f|=0x20;
        if(result>0xFFFF)
            reg.f|=0x10;
        reg.sethl(result);
    }
    void ld_BC(){
        //LD A,[BC]
        //TODO everything
        return;
    }
    void dec_bc(){
        reg.setbc(reg.getbc()-1);
    }
    void inc_c(){
        uint8_t result=reg.c+1;
        reg.f&=0x10;
        if(!result)
            reg.f|=0x80;
        if((reg.c&0xF)+1>0xF)
            reg.f|=0x20;
        reg.c=result;
    }
    void dec_c(){
        uint8_t result=reg.c-1;
        reg.f&=0x10;
        if(!result)
            reg.f|=0x80;
        reg.f|=0x40;
        if((reg.c&0xF)-1<0)
            reg.f|=0x20;
        reg.c=result;
    }
    void ldc_n(uint8_t n){
        reg.c=n;
    }
    void rrc_a(){
        int c=((reg.f&0x10)!=0);
        reg.f=0;
        reg.f|=((reg.a&0x1)<<4);
        reg.a>>=1;
        reg.a+=(c<<7);
        if(!reg.a)
            reg.f|=0x80;
    }
    //8x Instructions
    void add_a(){
        int result=reg.a+reg.a;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.a&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_b(){
        int result=reg.a+reg.b;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.b&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_c(){
        int result=reg.a+reg.c;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.c&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_d(){
        int result=reg.a+reg.d;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.d&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_e(){
        int result=reg.a+reg.e;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.e&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_h(){
        int result=reg.a+reg.h;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.h&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_l(){
        int result=reg.a+reg.l;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.l&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_hl(){
        //ADD A,[HL]
        //TODO fix this
        int result=reg.a+reg.l;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.l&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void add_n(uint8_t n){
        int result=reg.a+n;
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(n&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_a(){
        int result=reg.a+reg.a+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.a&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_b(){
        int result=reg.a+reg.b+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.b&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_c(){
        int result=reg.a+reg.c+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.c&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_d(){
        int result=reg.a+reg.d+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.d&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_e(){
        int result=reg.a+reg.e+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.e&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_h(){
        int result=reg.a+reg.h+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.h&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_l(){
        int result=reg.a+reg.l+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.l&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_hl(){
        //ADC A,[HL]
        //TODO fix this
        int result=reg.a+reg.l+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(reg.l&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void adc_n(uint8_t n){
        int result=reg.a+n+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(n&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
    }
    void printState(){        
        cout<<"a:"<<unsigned(reg.a)<<"\tf:"<<unsigned(reg.f)<<"\n";
        cout<<"b:"<<unsigned(reg.b)<<"\tc:"<<unsigned(reg.c)<<"\n";
        cout<<"d:"<<unsigned(reg.d)<<"\te:"<<unsigned(reg.e)<<"\n";
        cout<<"h:"<<unsigned(reg.h)<<"\tl:"<<unsigned(reg.l)<<"\n";
        cout<<"pc:"<<reg.pc<<"\tsp:"<<reg.sp<<"\n";
        cout<<"Flags\n";
        cout<<"z:"<<((reg.f&0x80)!=0)<<"\tn:"<<((reg.f&0x40)!=0)<<"\th:"<<((reg.f&0x20)!=0)<<"\tc:"<<((reg.f&0x10)!=0)<<"\n";
    }
};