#include "cpu.cpp"
int main(){
    cpu.reg.init();
    cpu.printState();
    cpu.reg.a=0x1;
    cpu.printState();
    cpu.rrc_a();
    cpu.printState();
    return 0;
}