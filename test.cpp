#include "cpu.cpp"
int main(){
    CPU cpu;
    cpu.reg.init();
    cpu.printState();
    cout<<"\n";
    cpu.reg.a=0x1;
    cpu.printState();
    cout<<"\n";
    cpu.rrc_a();
    cpu.printState();
    return 0;
}