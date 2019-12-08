#include "cpu.cpp"
int main(){
    cpu.ldsp(0xFFFE);
    cpu.printState();
    cpu.xor_a();
    cpu.printState();
    cpu.ldhl(0x9FFF);
    cpu.printState();
}