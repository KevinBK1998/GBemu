#include "registers.cpp"
#include "memory.cpp"
#include<iostream>
using namespace std;
struct CPU{
    registers reg;
    int m;
    int t;
    //0x Instructions
    void nop(){
        cout<<"NOP\n";
        m=1;
    }
    void ldbc(){
        //reg.setbc(n);
        reg.c=read8(reg.pc);
        reg.b=read8(reg.pc+1);
        reg.pc+=2;
        m=3;       
    }
    void ldBC_a(){
        //LD [BC],A
        write8(reg.getbc(),reg.a);
        m=2;
        return;
    }
    void inc_bc(){
        reg.setbc(reg.getbc()+1);
        m=1;
    }
    void inc_b(){
        uint8_t result=reg.b+1;
        reg.f&=0x10;
        if(!result)
            reg.f|=0x80;
        if((reg.b&0xF)+1>0xF)
            reg.f|=0x20;
        reg.b=result;
        m=1;
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
        m=1;
    }
    void ldb_n(uint8_t n){
        //reg.b=n;
        reg.b=read8(reg.pc);
        reg.pc++;
        m=2;
    }
    void rlc_a(){
        int c=((reg.f&0x10)!=0);
        reg.f=0;
        reg.f|=((reg.a&0x80)>>3);
        reg.a<<=1;
        reg.a+=c;
        if(!reg.a)
            reg.f|=0x80;
        m=2;
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
        m=3;
    }
    void ld_BC(){
        //LD A,[BC]
        reg.a=read8(reg.getbc());
        m=2;
    }
    void dec_bc(){
        reg.setbc(reg.getbc()-1);
        m=1;
    }
    void inc_c(){
        uint8_t result=reg.c+1;
        reg.f&=0x10;
        if(!result)
            reg.f|=0x80;
        if((reg.c&0xF)+1>0xF)
            reg.f|=0x20;
        reg.c=result;
        m=1;
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
        m=1;
    }
    void ldc_n(uint8_t n){
        //reg.c=n;
        reg.c=read8(reg.pc);
        reg.pc++;
        m=2;
    }
    void rrc_a(){
        int c=((reg.f&0x10)!=0);
        reg.f=0;
        reg.f|=((reg.a&0x1)<<4);
        reg.a>>=1;
        reg.a+=(c<<7);
        if(!reg.a)
            reg.f|=0x80;
        m=2;
    }
    //1x Instructions
    //2x Instructions
    void ldhl(uint16_t n){
        //reg.sethl(n);
        reg.l=read8(reg.pc);
        reg.h=read8(reg.pc+1);
        reg.pc+=2;
        m=3;  
    }
    //3x Instructions
    void ldsp(uint16_t n){
        //reg.setsp(n);
        reg.sp=read16(reg.pc);
        reg.pc+=2;
        m=3;  
    }
    void lddHL_a(){
        //LDD [HL],A
        write8(reg.gethl(),reg.a);
        m=2;
    }
    //8x Instructions
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
    }
    void add_HL(){
        //ADD A,[HL]
        int result=reg.a+read8(reg.gethl());
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(read8(reg.gethl())&0xF)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
        m=2;
    }
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
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
        m=1;
    }
    void adc_HL(){
        //ADC A,[HL]
        int result=reg.a+read8(reg.gethl())+((reg.f&0x10)!=0);
        reg.f=0;
        if(!(result&0xFF))
            reg.f|=0x80;
        if((reg.a&0xF)+(read8(reg.gethl())&0xF)+((reg.f&0x10)!=0)>0xF)
            reg.f|=0x20;
        if(result>0xFF)
            reg.f|=0x10;
        reg.a=result;
        m=2;
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
        m=1;
    }
    //9x Instructions
    //Ax Instructions
    void xor_a(){
        uint8_t res=reg.a^reg.a;
        reg.f=0;
        if(!res)
            reg.f|=0x80;
        reg.a=res;
    }
    //Cx Instructions
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
        cout<<"\na:"<<unsigned(reg.a)<<"\tf:"<<unsigned(reg.f)<<"\n";
        cout<<"b:"<<unsigned(reg.b)<<"\tc:"<<unsigned(reg.c)<<"\n";
        cout<<"d:"<<unsigned(reg.d)<<"\te:"<<unsigned(reg.e)<<"\n";
        cout<<"h:"<<unsigned(reg.h)<<"\tl:"<<unsigned(reg.l)<<"\n";
        cout<<"pc:"<<reg.pc<<"\tsp:"<<reg.sp<<"\n";
        cout<<"Flags\n";
        cout<<"z:"<<((reg.f&0x80)!=0)<<"\tn:"<<((reg.f&0x40)!=0)<<"\th:"<<((reg.f&0x20)!=0)<<"\tc:"<<((reg.f&0x10)!=0)<<"\n";
    }
}cpu;