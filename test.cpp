#include "cpu.h"
int main(){
    cpu.reg.init();
    cpu.reg.f=0x0;
    cpu.printState();
    cout<<endl;
    cpu.ccf();
    cpu.printState();
    return 0;
}