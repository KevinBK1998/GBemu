#include "registers.cpp"
#include "memory.cpp"
#include <iostream>
#include <stdlib.h>
using namespace std;
int m, ime, hlt;
unsigned long m_tot;
struct registers reg;
void reset(char *name)
{
    gpu.reset();
    mmu.reset();
    reg.reset();
    m = m_tot = 0;
    ime = 1;
    if (name[0] != '.')
        mmu.load(name);
    else
        mmu.load("ttt.gb");
    cout << hex << uppercase;
}
void printState()
{
    cout << "CPU:\n";
    cout << "a:" << unsigned(reg.a) << "\tf:" << unsigned(reg.f) << "\n";
    cout << "b:" << unsigned(reg.b) << "\tc:" << unsigned(reg.c) << "\n";
    cout << "d:" << unsigned(reg.d) << "\te:" << unsigned(reg.e) << "\n";
    cout << "h:" << unsigned(reg.h) << "\tl:" << unsigned(reg.l) << "\n";
    cout << "pc:" << reg.pc << "\tsp:" << reg.sp << "\n";
    cout << "z:" << ((reg.f & 0x80) != 0) << "\tn:" << ((reg.f & 0x40) != 0) << "\th:" << ((reg.f & 0x20) != 0) << "\tc:" << ((reg.f & 0x10) != 0) << "\time:" << ime << "\n";
    cout << "Time passed:" << dec << m_tot / 1000 << " kcycles" << hex << endl;
    gpu.printState();
}
void gpuStep()
{
    gpu.clk += 4 * m;
    switch (gpu.stat.mode)
    {
    case 0:
        if (gpu.clk >= 204)
        { //if hblank done go to oam
            gpu.clk = 0;
            gpu.line++;
            if (gpu.line == 143)
            { //if last line go to vblank after render screen
                gpu.stat.mode = 1;
                if (!gpu.ctrl.lcdOn)
                    gpu.clear();
                if (gpu.renScreen())
                    mmu.ifl |= 0x10; //Joypad
                mmu.ifl |= 1;        //Vblank
            }
            else
            {
                gpu.stat.mode = 2;
            }
        }
        break;
    case 1:
        if (gpu.clk >= 456)
        { //wait for 10 lines
            gpu.clk = 0;
            gpu.line++;
            if (gpu.line == 153)
            { //if vblank done go to oam(repeat)
                gpu.stat.mode = 2;
                gpu.line = 0;
            }
        }
        break;
    case 2:
        if (gpu.clk >= 80)
        { //if oam done go to vram
            gpu.stat.mode = 3;
            gpu.clk = 0;
        }
        break;
    case 3:
        if (gpu.clk >= 172)
        { //if vram done scanline and enter hblank
            gpu.stat.mode = 0;
            gpu.clk = 0;
            if (gpu.ctrl.lcdOn)
                gpu.scanline();
        }
        break;
    default:
        cout << "gpu time error\n";
    }
}
void checkInt()
{
    if (ime && mmu.ie && mmu.ifl)
    {
        uint8_t fired = mmu.ie & mmu.ifl;
        cout << "IE:0x" << unsigned(mmu.ie) << " IF:0x" << unsigned(mmu.ifl) << endl;
        ime = 0;
        hlt = 0;
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc);
        m = 3;
        if (fired & 1)
        {
            cout << "INT VB\n";
            mmu.ifl &= 0xFE;
            reg.pc = 0x40;
        }
        else if (fired & 2)
        {
            cout << "INT LCD\n";
            mmu.ifl &= 0xFD;
            reg.pc = 0x48;
        }
        else if (fired & 4)
        {
            cout << "INT TIM\n";
            mmu.ifl &= 0xFB;
            reg.pc = 0x50;
        }
        else if (fired & 8)
        {
            cout << "INT SER\n";
            mmu.ifl &= 0xF7;
            reg.pc = 0x58;
        }
        else if (fired & 0x10)
        {
            cout << "INT JP\n";
            mmu.ifl &= 0xEF;
            reg.pc = 0x60;
        }
        else
        {
            ime = 1;
            reg.sp += 2;
            m = 0;
        }
    }
}
int main(int argc, char *args[])
{
    char ch = 'c';
    reset(args[argc - 1]);
    int res;
    uint8_t op2, result;
    Uint16 nn;
    int8_t n;
    while (1)
    {
        if (hlt)
            m = 1;
        else
        {
            uint8_t op = mmu.read8(reg.pc++);
            switch (op)
            {
            case 0x00:
                cout << "NOP\n";
                m = 1;
                break;
            case 0x01:
                cout << "LD BC,0x" << unsigned(mmu.read16(reg.pc)) << endl;
                reg.setbc(mmu.read16(reg.pc));
                reg.pc += 2;
                m = 3;
                break;
            case 0x03:
                cout << "INC BC\n";
                reg.setbc(reg.getbc() + 1);
                m = 1;
                break;
            case 0x04:
                cout << "INC B\n";
                result = reg.b + 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                if ((reg.b & 0xF) + 1 > 0xF)
                    reg.f |= 0x20;
                reg.b = result;
                m = 1;
                break;
            case 0x05:
                cout << "DEC B\n";
                result = reg.b - 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.b & 0xF) - 1 < 0)
                    reg.f |= 0x20;
                reg.b = result;
                m = 1;
                break;
            case 0x06:
                cout << "LD B,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.b = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x07:
                cout << "RLC A\n";
                reg.f = 0;
                reg.f |= ((reg.a & 0x80) >> 3);
                reg.a <<= 1;
                reg.a += ((reg.f & 0x10) != 0);
                if (!reg.a)
                    reg.f |= 0x80;
                m = 1;
                break;
            case 0x09:
                cout << "ADD HL,BC\n";
                res = reg.gethl() + reg.getbc();
                reg.f &= 0x80;
                if ((reg.gethl() & 0xFFF) + (reg.getbc() & 0xFFF) > 0xFFF)
                    reg.f |= 0x20;
                if (res > 0xFFFF)
                    reg.f |= 0x10;
                reg.sethl(res);
                m = 3;
                break;
            case 0x0A:
                cout << "LD A,[BC]\n";
                reg.a = mmu.read8(reg.getbc());
                m = 2;
                break;
            case 0x0B:
                cout << "DEC BC\n";
                reg.setbc(reg.getbc() - 1);
                m = 1;
                break;
            case 0x0C:
                cout << "INC C\n";
                result = reg.c + 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                if ((reg.c & 0xF) + 1 > 0xF)
                    reg.f |= 0x20;
                reg.c = result;
                m = 1;
                break;
            case 0x0D:
                cout << "DEC C\n";
                result = reg.c - 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.c & 0xF) - 1 < 0)
                    reg.f |= 0x20;
                reg.c = result;
                m = 1;
                break;
            case 0x0E:
                cout << "LD C,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.c = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x11:
                cout << "LD DE,0x" << unsigned(mmu.read16(reg.pc)) << endl;
                reg.setde(mmu.read16(reg.pc));
                reg.pc += 2;
                m = 3;
                break;
            case 0x12:
                cout << "LD [DE],A\n";
                mmu.write8(reg.getde(), reg.a);
                m = 2;
                break;
            case 0x13:
                cout << "INC DE\n";
                reg.setde(reg.getde() + 1);
                m = 1;
                break;
            case 0x15:
                cout << "DEC D\n";
                result = reg.d - 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.d & 0xF) - 1 < 0)
                    reg.f |= 0x20;
                reg.d = result;
                m = 1;
                break;
            case 0x16:
                cout << "LD D,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.d = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x17:
                cout << "RL A\n";
                res = ((reg.f & 0x10) != 0);
                reg.f = 0;
                reg.f |= ((reg.a & 0x80) >> 3);
                reg.a <<= 1;
                reg.a += res;
                if (!reg.a)
                    reg.f |= 0x80;
                m = 1;
                break;
            case 0x18:
                n = mmu.read8(reg.pc++);
                cout << "JR PC+" << dec << int(n) << hex << "\n";
                reg.pc += n;
                m = 3;
                break;
            case 0x19:
                cout << "ADD HL,DE\n";
                res = reg.gethl() + reg.getde();
                reg.f &= 0x80;
                if ((reg.gethl() & 0xFFF) + (reg.getde() & 0xFFF) > 0xFFF)
                    reg.f |= 0x20;
                if (res > 0xFFFF)
                    reg.f |= 0x10;
                reg.sethl(res);
                m = 3;
                break;
            case 0x1A:
                cout << "LD A,[DE]\n";
                reg.a = mmu.read8(reg.getde());
                m = 2;
                break;
            case 0x1B:
                cout << "DEC DE\n";
                reg.setde(reg.getde() - 1);
                m = 1;
                break;
            case 0x1D:
                cout << "DEC E\n";
                result = reg.e - 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.e & 0xF) - 1 < 0)
                    reg.f |= 0x20;
                reg.e = result;
                m = 1;
                break;
            case 0x1E:
                cout << "LD E,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.e = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x20:
                n = mmu.read8(reg.pc++);
                cout << "JR NZ,PC+" << dec << int(n) << hex << "\n";
                m = 2;
                if (!(reg.f & 0x80))
                {
                    reg.pc += n;
                    m++;
                }
                break;
            case 0x21:
                cout << "LD HL,0x" << unsigned(mmu.read16(reg.pc)) << endl;
                reg.sethl(mmu.read16(reg.pc));
                reg.pc += 2;
                m = 3;
                break;
            case 0x22:
                cout << "LDI [HL],A\n";
                mmu.write8(reg.gethl(), reg.a);
                reg.sethl(reg.gethl() + 1);
                m = 2;
                break;
            case 0x23:
                cout << "INC HL\n";
                reg.sethl(reg.gethl() + 1);
                m = 1;
                break;
            case 0x24:
                cout << "INC H\n";
                result = reg.h + 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                if ((reg.h & 0xF) + 1 > 0xF)
                    reg.f |= 0x20;
                reg.h = result;
                m = 1;
                break;
            case 0x26:
                cout << "LD H,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.h = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x28:
                n = mmu.read8(reg.pc++);
                cout << "JR Z,PC+" << dec << int(n) << hex << "\n";
                m = 2;
                if (reg.f & 0x80)
                {
                    reg.pc += n;
                    m++;
                }
                break;
            case 0x29:
                cout << "ADD HL,HL\n";
                result = reg.gethl() + reg.gethl();
                reg.f &= 0x80;
                if ((reg.gethl() & 0xFFF) + (reg.gethl() & 0xFFF) > 0xFFF)
                    reg.f |= 0x20;
                if (result > 0xFFFF)
                    reg.f |= 0x10;
                reg.sethl(result);
                m = 3;
                break;
            case 0x2B:
                cout << "DEC HL\n";
                reg.sethl(reg.gethl() - 1);
                m = 1;
                break;
            case 0x2E:
                cout << "LD L,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.l = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x2F:
                cout << "CPL\n";
                reg.a = ~reg.a;
                reg.f |= 0x60;
                m = 2;
                break;
            case 0x30:
                cout << "JR NC,PC+" << dec << int(n) << hex << "\n";
                n = mmu.read8(reg.pc++);
                m = 2;
                if (!(reg.f & 0x10))
                {
                    reg.pc += n;
                    m++;
                }
                break;
            case 0x31:
                cout << "LD SP,0x" << unsigned(mmu.read16(reg.pc)) << endl;
                reg.sp = mmu.read16(reg.pc);
                reg.pc += 2;
                m = 3;
                break;
            case 0x32:
                cout << "LDD [HL],A\n";
                mmu.write8(reg.gethl(), reg.a);
                reg.sethl(reg.gethl() - 1);
                m = 2;
                break;
            case 0x37:
                cout << "SCF\n";
                reg.f &= 0x80;
                reg.f |= 0x10;
                m = 1;
                break;
            case 0x39:
                cout << "ADD HL,SP\n";
                res = reg.gethl() + reg.sp;
                reg.f &= 0x80;
                if ((reg.gethl() & 0xFFF) + (reg.sp & 0xFFF) > 0xFFF)
                    reg.f |= 0x20;
                if (res > 0xFFFF)
                    reg.f |= 0x10;
                reg.sethl(res);
                m = 3;
                break;
            case 0x3D:
                cout << "DEC A\n";
                result = reg.a - 1;
                reg.f &= 0x10;
                if (!result)
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.a & 0xF) - 1 < 0)
                    reg.f |= 0x20;
                reg.a = result;
                m = 1;
                break;
            case 0x3E:
                cout << "LD A,0x" << unsigned(mmu.read8(reg.pc)) << endl;
                reg.a = mmu.read8(reg.pc);
                reg.pc++;
                m = 2;
                break;
            case 0x44:
                cout << "LD B,H\n";
                reg.b = reg.h;
                m = 1;
                break;
            case 0x47:
                cout << "LD B,A\n";
                reg.b = reg.a;
                m = 1;
                break;
            case 0x4D:
                cout << "LD C,L\n";
                reg.c = reg.l;
                m = 1;
                break;
            case 0x4F:
                cout << "LD C,A\n";
                reg.c = reg.a;
                m = 1;
                break;
            case 0x55:
                cout << "LD D,L\n";
                reg.d = reg.l;
                m = 1;
                break;
            case 0x57:
                cout << "LD D,A\n";
                reg.d = reg.a;
                m = 1;
                break;
            case 0x5D:
                cout << "LD E,L\n";
                reg.e = reg.l;
                m = 1;
                break;
            case 0x5F:
                cout << "LD E,A\n";
                reg.e = reg.a;
                m = 1;
                break;
            case 0x66:
                cout << "LD H,[HL]\n";
                reg.h = mmu.read8(reg.gethl());
                m = 2;
                break;
            case 0x67:
                cout << "LD H,A\n";
                reg.h = reg.a;
                m = 1;
                break;
            case 0x6F:
                cout << "LD L,A\n";
                reg.l = reg.a;
                m = 1;
                break;
            case 0x77:
                cout << "LD [HL],A\n";
                mmu.write8(reg.gethl(), reg.a);
                m = 2;
                break;
            case 0x78:
                cout << "LD A,B\n";
                reg.a = reg.b;
                m = 1;
                break;
            case 0x7A:
                cout << "LD A,D\n";
                reg.a = reg.d;
                m = 1;
                break;
            case 0x7B:
                cout << "LD A,E\n";
                reg.a = reg.e;
                m = 1;
                break;
            case 0x7C:
                cout << "LD A,H\n";
                reg.a = reg.h;
                m = 1;
                break;
            case 0x7D:
                cout << "LD A,L\n";
                reg.a = reg.l;
                m = 1;
                break;
            case 0x7E:
                cout << "LD A,[HL]\n";
                reg.a = mmu.read8(reg.gethl());
                m = 2;
                break;
            case 0x86:
                cout << "ADD A,[HL]\n";
                res = reg.a + mmu.read8(reg.gethl());
                reg.f = 0;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                if ((reg.a & 0xF) + (mmu.read8(reg.gethl()) & 0xF) > 0xF)
                    reg.f |= 0x20;
                if (res > 0xFF)
                    reg.f |= 0x10;
                reg.a = res;
                m = 2;
                break;
            case 0x87:
                cout << "ADD A,A\n";
                res = reg.a + reg.a;
                reg.f = 0;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                if ((reg.a & 0xF) + (reg.a & 0xF) > 0xF)
                    reg.f |= 0x20;
                if (res > 0xFF)
                    reg.f |= 0x10;
                reg.a = res;
                m = 1;
                break;
            case 0x90:
                cout << "SUB A,B\n";
                res = reg.a - reg.b;
                reg.f = 0;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.a & 0xF) - (reg.b & 0xF) < 0)
                    reg.f |= 0x20;
                if (res < 0)
                    reg.f |= 0x10;
                reg.a = res;
                m = 1;
                break;
            case 0x95:
                cout << "SUB A,L\n";
                res = reg.a - reg.l;
                reg.f = 0;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.a & 0xF) - (reg.l & 0xF) < 0)
                    reg.f |= 0x20;
                if (res < 0)
                    reg.f |= 0x10;
                reg.a = res;
                m = 1;
                break;
            case 0x97:
                cout << "SUB A,A\n";
                reg.f = 0xC0;
                reg.a = 0;
                m = 1;
                break;
            case 0x9C:
                cout << "SBC A,H\n";
                res = reg.a - (reg.h + ((reg.f & 0x10) != 0));
                reg.f = 0;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.a & 0xF) - ((reg.h & 0xF) + ((reg.f & 0x10) != 0)) < 0)
                    reg.f |= 0x20;
                if (res < 0)
                    reg.f |= 0x10;
                reg.a = res;
                m = 1;
                break;
            case 0x9F:
                cout << "SBC A,A\n";
                res = -((reg.f & 0x10) != 0);
                reg.f = 0;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                reg.f |= 0x40;
                if ((reg.a & 0xF) - ((reg.a & 0xF) + ((reg.f & 0x10) != 0)) < 0)
                    reg.f |= 0x20;
                if (res < 0)
                    reg.f |= 0x10;
                reg.a = res;
                m = 1;
                break;
            case 0xA2:
                cout << "AND D\n";
                result = reg.a & reg.d;
                reg.f = 0x20;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xA3:
                cout << "AND E\n";
                result = reg.a & reg.e;
                reg.f = 0x20;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xAF:
                cout << "XOR A,A\n";
                reg.a = 0;
                reg.f = 0x80;
                m = 1;
                break;
            case 0xB0:
                cout << "OR B\n";
                result = reg.a | reg.b;
                reg.f = 0;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xB1:
                cout << "OR C\n";
                result = reg.a | reg.c;
                reg.f = 0;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xB3:
                cout << "OR E\n";
                result = reg.a | reg.e;
                reg.f = 0;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xB4:
                cout << "OR H\n";
                result = reg.a | reg.h;
                reg.f = 0;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xB5:
                cout << "OR L\n";
                result = reg.a | reg.l;
                reg.f = 0;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                m = 1;
                break;
            case 0xB7:
                cout << "OR A\n";
                reg.f = 0;
                if (!reg.a)
                    reg.f |= 0x80;
                m = 1;
                break;
            case 0xBE:
                cout << "CP [HL]\n";
                res = reg.a - mmu.read8(reg.gethl());
                reg.f = 0x40;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                if ((reg.a & 0xF) - (mmu.read8(reg.gethl()) & 0xF) < 0)
                    reg.f |= 0x20;
                if (res < 0)
                    reg.f |= 0x10;
                m = 2;
                break;
            case 0xC0:
                cout << "RET NZ\n";
                m = 1;
                if (!(reg.f & 0x80))
                {
                    reg.pc = mmu.read16(reg.sp);
                    reg.sp += 2;
                    m += 2;
                }
                break;
            case 0xC1:
                cout << "POP BC\n";
                reg.setbc(mmu.read16(reg.sp));
                reg.sp += 2;
                m = 3;
                break;
            case 0xC2:
                cout << "JP NZ,0x" << unsigned(mmu.read16(reg.pc)) << "\n";
                nn = mmu.read16(reg.pc);
                reg.pc += 2;
                m = 3; //doubt
                if (!(reg.f & 0x80))
                {
                    reg.pc = nn;
                    m++;
                }
                break;
            case 0xC3:
                cout << "JP 0x" << unsigned(mmu.read16(reg.pc)) << "\n";
                reg.pc = mmu.read16(reg.pc);
                m = 3;
                break;
            case 0xC5:
                cout << "PUSH BC\n";
                reg.sp -= 2;
                mmu.write16(reg.sp, reg.getbc());
                m = 3;
                break;
            case 0xC8:
                cout << "RET Z\n";
                m = 1;
                if (reg.f & 0x80)
                {
                    reg.pc = mmu.read16(reg.sp);
                    reg.sp += 2;
                    m += 2;
                }
                break;
            case 0xC9:
                cout << "RET\n";
                reg.pc = mmu.read16(reg.sp);
                reg.sp += 2;
                m = 3;
                break;
            case 0xCA:
                cout << "JP Z,0x" << unsigned(mmu.read16(reg.pc)) << "\n";
                nn = mmu.read16(reg.pc);
                reg.pc += 2;
                m = 3; //doubt
                if (reg.f & 0x80)
                {
                    reg.pc = nn;
                    m++;
                }
                break;
            case 0xCB:
                cout << "0xCB:";
                op2 = mmu.read8(reg.pc++);
                switch (op2)
                {
                case 0x01:
                    cout << "RLC C\n";
                    reg.f = 0;
                    reg.f |= ((reg.c & 0x80) >> 3);
                    reg.c <<= 1;
                    reg.c += ((reg.f & 0x10) != 0);
                    if (!reg.c)
                        reg.f |= 0x80;
                    m = 2;
                    break;
                case 0x11:
                    cout << "RL C\n";
                    res = ((reg.f & 0x10) != 0);
                    reg.f = 0;
                    reg.f |= ((reg.c & 0x80) >> 3);
                    reg.c <<= 1;
                    reg.c += res;
                    if (!reg.c)
                        reg.f |= 0x80;
                    m = 2;
                    break;
                case 0x37:
                    cout << "SWAP A\n";
                    reg.f = 0;
                    result = (reg.a & 0xF) << 4;
                    reg.a >>= 4;
                    reg.a += result;
                    if (!reg.a)
                        reg.f |= 0x80;
                    m = 2;
                    break;
                case 0x7C:
                    cout << "BIT 7,H\n";
                    reg.f &= 0x10;
                    reg.f |= 0x20;
                    if (!(reg.h >> 7))
                        reg.f |= 0x80;
                    m = 2;
                    break;
                default:
                    printState();
                    mmu.dump();
                    cout << "Unknown Instruction 0xCB 0x" << unsigned(op2) << "\n";
                    exit(1);
                }
                break;
            case 0xCD:
                cout << "CALL 0x" << unsigned(mmu.read16(reg.pc)) << "\n";
                nn = mmu.read16(reg.pc);
                reg.sp -= 2;
                mmu.write16(reg.sp, reg.pc + 2);
                reg.pc = nn;
                m = 5;
                break;
            case 0xD0:
                cout << "RET NC\n";
                m = 1;
                if (!(reg.f & 0x10))
                {
                    reg.pc = mmu.read16(reg.sp);
                    reg.sp += 2;
                    m += 2;
                }
                break;
            case 0xD1:
                cout << "POP DE\n";
                reg.setde(mmu.read16(reg.sp));
                reg.sp += 2;
                m = 3;
                break;
            case 0xD5:
                cout << "PUSH DE\n";
                reg.sp -= 2;
                mmu.write16(reg.sp, reg.getde());
                m = 3;
                break;
            case 0xD8:
                cout << "RET C\n";
                m = 1;
                if (reg.f & 0x10)
                {
                    reg.pc = mmu.read16(reg.sp);
                    reg.sp += 2;
                    m += 2;
                }
                break;
            case 0xE0:
                cout << "LD [0xFF 0x" << unsigned(mmu.read8(reg.pc)) << "],A\n";
                mmu.write8(0xFF00 + mmu.read8(reg.pc), reg.a);
                reg.pc++;
                m = 3;
                break;
            case 0xE1:
                cout << "POP HL\n";
                reg.sethl(mmu.read16(reg.sp));
                reg.sp += 2;
                m = 3;
                break;
            case 0xE2:
                cout << "LD [0xFF00+C],A\n";
                mmu.write8(0xFF00 + reg.c, reg.a);
                m = 2;
                break;
            case 0xE5:
                cout << "PUSH HL\n";
                reg.sp -= 2;
                mmu.write16(reg.sp, reg.gethl());
                m = 3;
                break;
            case 0xE6:
                cout << "AND 0x" << unsigned(mmu.read8(reg.pc)) << endl;
                result = reg.a & mmu.read8(reg.pc);
                reg.f = 0x20;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                reg.pc++;
                m = 2;
                break;
            case 0xE8:
                n = mmu.read8(reg.pc);
                cout << "ADD SP," << dec << 0 + n << hex << "\n";
                reg.pc++;
                reg.sp += n;
                m = 1;
                break;
            case 0xEA:
                cout << "LD [0x" << unsigned(mmu.read16(reg.pc)) << "],A\n";
                mmu.write8(mmu.read16(reg.pc), reg.a);
                reg.pc += 2;
                m = 4;
                break;
            case 0xF0:
                cout << "LD A,[0xFF 0x" << unsigned(mmu.read8(reg.pc)) << "]\n";
                reg.a = mmu.read8(0xFF00 + mmu.read8(reg.pc));
                reg.pc++;
                m = 3;
                break;
            case 0xF2:
                cout << "LD A,[0xFF00+C]\n";
                reg.a = mmu.read8(0xFF00 + reg.c);
                m = 2;
                break;
            case 0xF3:
                cout << "DI\n";
                ime = 0;
                m = 1;
                break;
            case 0xF6:
                cout << "OR 0x" << unsigned(mmu.read8(reg.pc)) << "\n";
                result = reg.a | mmu.read8(reg.pc);
                reg.f = 0;
                if (!result)
                    reg.f |= 0x80;
                reg.a = result;
                reg.pc++;
                m = 2;
                break;
            case 0xFA:
                cout << "LD A,[0x" << unsigned(mmu.read16(reg.pc)) << "]\n";
                reg.a = mmu.read8(mmu.read16(reg.pc));
                reg.pc += 2;
                m = 4;
                break;
            case 0xFE:
                cout << "CP 0x" << unsigned(mmu.read8(reg.pc)) << "\n";
                res = reg.a - mmu.read8(reg.pc);
                reg.f = 0x40;
                if (!(res & 0xFF))
                    reg.f |= 0x80;
                if ((reg.a & 0xF) - (mmu.read8(reg.pc) & 0xF) < 0)
                    reg.f |= 0x20;
                if (res < 0)
                    reg.f |= 0x10;
                reg.pc++;
                m = 2;
                break;
            default:
                printState();
                cout << "JOYP:\nRow1:" << unsigned(joyp.row[0]) << "\tRow2:" << unsigned(joyp.row[1]) << endl;
                mmu.dump();
                mmu.dumpset();
                mmu.dumpmap();
                gpu.dumpoam();
                cout << "Unknown Instruction 0x" << unsigned(op) << "\n";
                exit(1);
            }
            m_tot += m;
            gpuStep();
        } //

        m = 0;
        checkInt();
        m_tot += m;
        if (m)
            gpuStep(); //
    }
}