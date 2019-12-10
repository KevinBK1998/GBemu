#include "registers.cpp"
#include "memory.cpp"
#include <iostream>
#include <stdlib.h>
using namespace std;
struct CPU
{
    registers reg;
    int m, m_tot;
    int t, t_tot;
    //0x Instructions
    void nop()
    {
        m = 1;
    }
    void ldbc()
    {
        reg.c = mmu.read8(reg.pc);
        reg.b = mmu.read8(reg.pc + 1);
        reg.pc += 2;
        m = 3;
    }
    void ldBC_a()
    {
        //LD [BC],A
        mmu.write8(reg.getbc(), reg.a);
        m = 2;
    }
    void inc_bc()
    {
        reg.setbc(reg.getbc() + 1);
        m = 1;
    }
    void inc_b()
    {
        uint8_t result = reg.b + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.b & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.b = result;
        m = 1;
    }
    void dec_b()
    {
        uint8_t result = reg.b - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.b & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.b = result;
        m = 1;
    }
    void ldb_n()
    {
        reg.b = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void rlc_a()
    {
        reg.f = 0;
        reg.f |= ((reg.a & 0x80) >> 3);
        reg.a <<= 1;
        if (!reg.a)
            reg.f |= 0x80;
        m = 1;
    }
    void ldM_sp()
    {
        //LD [nnnn],SP
        mmu.write16(mmu.read16(reg.pc), reg.sp);
        reg.pc += 2;
        ;
    }
    void addhl_bc()
    {
        int result = reg.gethl() + reg.getbc();
        reg.f &= 0x80;
        if ((reg.gethl() & 0xFFF) + (reg.getbc() & 0xFFF) > 0xFFF)
            reg.f |= 0x20;
        if (result > 0xFFFF)
            reg.f |= 0x10;
        reg.sethl(result);
        m = 3;
    }
    void ld_BC()
    {
        //LD A,[BC]
        reg.a = mmu.read8(reg.getbc());
        m = 2;
    }
    void dec_bc()
    {
        reg.setbc(reg.getbc() - 1);
        m = 1;
    }
    void inc_c()
    {
        uint8_t result = reg.c + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.c & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.c = result;
        m = 1;
    }
    void dec_c()
    {
        uint8_t result = reg.c - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.c & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.c = result;
        m = 1;
    }
    void ldc_n()
    {
        reg.c = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void rrc_a()
    {
        reg.f = 0;
        reg.f |= ((reg.a & 0x1) << 4);
        reg.a >>= 1;
        if (!reg.a)
            reg.f |= 0x80;
        m = 2;
    }
    //1x Instructions
    void stop()
    {
        //stop or jump?
        //Ignore for now
        return;
    }
    void ldde()
    {
        reg.e = mmu.read8(reg.pc);
        reg.d = mmu.read8(reg.pc + 1);
        reg.pc += 2;
        m = 3;
    }
    void ldDE_a()
    {
        //LD [DE],A
        mmu.write8(reg.getde(), reg.a);
        m = 2;
    }
    void inc_de()
    {
        reg.setde(reg.getde() + 1);
        m = 1;
    }
    void inc_d()
    {
        uint8_t result = reg.d + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.d & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.d = result;
        m = 1;
    }
    void dec_d()
    {
        uint8_t result = reg.d - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.d & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.d = result;
        m = 1;
    }
    void ldd_n()
    {
        reg.d = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void rl_a()
    {
        int c = ((reg.f & 0x10) != 0);
        reg.f = 0;
        reg.f |= ((reg.a & 0x80) >> 3);
        reg.a <<= 1;
        reg.a += c;
        if (!reg.a)
            reg.f |= 0x80;
        m = 1;
    }
    void jr_n()
    {
        //JR N
        int8_t n = mmu.read8(reg.pc++);
        reg.pc += n;
        m = 3;
    }
    void addhl_de()
    {
        int result = reg.gethl() + reg.getde();
        reg.f &= 0x80;
        if ((reg.gethl() & 0xFFF) + (reg.getde() & 0xFFF) > 0xFFF)
            reg.f |= 0x20;
        if (result > 0xFFFF)
            reg.f |= 0x10;
        reg.sethl(result);
        m = 3;
    }
    void ld_DE()
    {
        //LD A,[DE]
        reg.a = mmu.read8(reg.getde());
        m = 2;
    }
    void dec_de()
    {
        reg.setde(reg.getde() - 1);
        m = 1;
    }
    void inc_e()
    {
        uint8_t result = reg.e + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.e & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.e = result;
        m = 1;
    }
    void dec_e()
    {
        uint8_t result = reg.e - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.e & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.e = result;
        m = 1;
    }
    void lde_n()
    {
        reg.e = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void rr_a()
    {
        int c = ((reg.f & 0x10) != 0);
        reg.f = 0;
        reg.f |= ((reg.a & 0x1) << 4);
        reg.a >>= 1;
        reg.a += (c << 7);
        if (!reg.a)
            reg.f |= 0x80;
        m = 2;
    }
    //2x Instructions
    void jrnz_n()
    {
        //JR NZ,N
        int8_t n = mmu.read8(reg.pc++);
        m = 2;
        if (!(reg.f & 0x80))
        {
            reg.pc += n;
            m++;
        }
    }
    void ldhl()
    {
        reg.l = mmu.read8(reg.pc);
        reg.h = mmu.read8(reg.pc + 1);
        cout << reg.gethl() << endl;
        reg.pc += 2;
        m = 3;
    }
    void ldHLi_a_()
    {
        //LDI [HL],A
        mmu.write8(reg.gethl(), reg.a);
        reg.sethl(reg.gethl() + 1);
        m = 2;
    }
    void inc_hl()
    {
        reg.sethl(reg.gethl() + 1);
        m = 1;
    }
    void inc_h()
    {
        uint8_t result = reg.h + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.h & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.h = result;
        m = 1;
    }
    void dec_h()
    {
        uint8_t result = reg.h - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.h & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.h = result;
        m = 1;
    }
    void ldh_n()
    {
        reg.h = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void daa()
    {
        //DAA  ->BCD adjust
        if ((reg.f & 0x20) || ((reg.a & 0xF) > 9))
            reg.a += 6;
        reg.f &= 0x50;
        if ((reg.f & 0x10) || ((reg.a & 0xF0) > 0x99))
        {
            reg.a += 0x60;
            reg.f |= 0x10;
        }
        if (!reg.a)
            reg.f |= 0x80;
        m = 1;
    }
    void jrz_n()
    {
        //JR Z,N
        int8_t n = mmu.read8(reg.pc++);
        m = 2;
        if (reg.f & 0x80)
        {
            reg.pc += n;
            m++;
        }
    }
    void addhl_hl()
    {
        int result = reg.gethl() + reg.gethl();
        reg.f &= 0x80;
        if ((reg.gethl() & 0xFFF) + (reg.gethl() & 0xFFF) > 0xFFF)
            reg.f |= 0x20;
        if (result > 0xFFFF)
            reg.f |= 0x10;
        reg.sethl(result);
        m = 3;
    }
    void ldi_HL()
    {
        //LDI A,[HL]
        reg.a = mmu.read8(reg.gethl());
        reg.sethl(reg.gethl() + 1);
        m = 2;
    }
    void dec_hl()
    {
        reg.sethl(reg.gethl() - 1);
        m = 1;
    }
    void inc_l()
    {
        uint8_t result = reg.l + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.l & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.l = result;
        m = 1;
    }
    void dec_l()
    {
        uint8_t result = reg.l - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.l & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.l = result;
        m = 1;
    }
    void ldl_n()
    {
        reg.l = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void cpl()
    {
        //CPL
        reg.a = ~reg.a;
        reg.f |= 0x60;
        m = 2;
    }
    //3x Instructions
    void jrnc_n()
    {
        //JR NC,N
        int8_t n = mmu.read8(reg.pc++);
        m = 2;
        if (!(reg.f & 0x10))
        {
            reg.pc += n;
            m++;
        }
    }
    void ldsp()
    {
        reg.sp = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3;
    }
    void ldHLd_a()
    {
        //LDD [HL],A
        mmu.write8(reg.gethl(), reg.a);
        reg.sethl(reg.gethl() - 1);
        m = 2;
    }
    void inc_sp()
    {
        reg.sp = reg.gethl() + 1;
        m = 1;
    }
    void inc_HL()
    {
        uint8_t result = mmu.read8(reg.gethl()) + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((mmu.read8(reg.gethl()) & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        mmu.write8(reg.gethl(), result);
        m = 3;
    }
    void dec_HL()
    {
        uint8_t result = mmu.read8(reg.gethl()) - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((mmu.read8(reg.gethl()) & 0xF) - 1 < 0)
            reg.f |= 0x20;
        mmu.write8(reg.gethl(), result);
        m = 3;
    }
    void ldHL_n()
    {
        mmu.write8(reg.gethl(), mmu.read8(reg.pc));
        reg.pc++;
        m = 3;
    }
    void scf()
    {
        //SCF
        reg.f &= 0x80;
        reg.f |= 0x10;
        m = 1;
    }
    void jrc_n()
    {
        //JR C,N
        int8_t n = mmu.read8(reg.pc++);
        m = 2;
        if (reg.f & 0x10)
        {
            reg.pc += n;
            m++;
        }
    }
    void addhl_sp()
    {
        int result = reg.gethl() + reg.sp;
        reg.f &= 0x80;
        if ((reg.gethl() & 0xFFF) + (reg.sp & 0xFFF) > 0xFFF)
            reg.f |= 0x20;
        if (result > 0xFFFF)
            reg.f |= 0x10;
        reg.sethl(result);
        m = 3;
    }
    void ldd_HL()
    {
        //LDD A,[HL]
        reg.a = mmu.read8(reg.gethl());
        reg.sethl(reg.gethl() - 1);
        m = 2;
    }
    void dec_sp()
    {
        reg.sp--;
        m = 1;
    }
    void inc_a()
    {
        uint8_t result = reg.a + 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        if ((reg.a & 0xF) + 1 > 0xF)
            reg.f |= 0x20;
        reg.a = result;
        m = 1;
    }
    void dec_a()
    {
        uint8_t result = reg.a - 1;
        reg.f &= 0x10;
        if (!result)
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - 1 < 0)
            reg.f |= 0x20;
        reg.a = result;
        m = 1;
    }
    void ld_n()
    {
        reg.a = mmu.read8(reg.pc);
        reg.pc++;
        m = 2;
    }
    void ccf()
    {
        //CCF
        reg.f &= 0x90;
        reg.f ^= 0x10;
        m = 1;
    }
    //8x Instructions
    void add_b()
    {
        int result = reg.a + reg.b;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.b & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void add_c()
    {
        int result = reg.a + reg.c;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.c & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void add_d()
    {
        int result = reg.a + reg.d;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.d & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void add_e()
    {
        int result = reg.a + reg.e;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.e & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void add_h()
    {
        int result = reg.a + reg.h;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.h & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void add_l()
    {
        int result = reg.a + reg.l;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.l & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void add_HL()
    {
        //ADD A,[HL]
        int result = reg.a + mmu.read8(reg.gethl());
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (mmu.read8(reg.gethl()) & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 2;
    }
    void add_a()
    {
        int result = reg.a + reg.a;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.a & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_b()
    {
        int result = reg.a + reg.b + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.b & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_c()
    {
        int result = reg.a + reg.c + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.c & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_d()
    {
        int result = reg.a + reg.d + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.d & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_e()
    {
        int result = reg.a + reg.e + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.e & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_h()
    {
        int result = reg.a + reg.h + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.h & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_l()
    {
        int result = reg.a + reg.l + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.l & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void adc_HL()
    {
        //ADC A,[HL]
        int result = reg.a + mmu.read8(reg.gethl()) + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (mmu.read8(reg.gethl()) & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 2;
    }
    void adc_a()
    {
        int result = reg.a + reg.a + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (reg.a & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    //9x Instructions
    //Ax Instructions
    void xor_a()
    {
        uint8_t res = reg.a ^ reg.a;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 2;
    }
    //Cx Instructions
    void add_n()
    {
        uint8_t n = mmu.read8(reg.pc);
        int result = reg.a + n;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (n & 0xF) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        reg.pc++;
        m = 2;
    }
    void adc_n()
    {
        uint8_t n = mmu.read8(reg.pc);
        int result = reg.a + n + ((reg.f & 0x10) != 0);
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) + (n & 0xF) + ((reg.f & 0x10) != 0) > 0xF)
            reg.f |= 0x20;
        if (result > 0xFF)
            reg.f |= 0x10;
        reg.a = result;
        reg.pc++;
        m = 2;
    }
    void printState()
    {
        cout << "a:" << hex << uppercase << unsigned(reg.a) << "\tf:" << unsigned(reg.f) << "\n";
        cout << "b:" << unsigned(reg.b) << "\tc:" << unsigned(reg.c) << "\n";
        cout << "d:" << unsigned(reg.d) << "\te:" << unsigned(reg.e) << "\n";
        cout << "h:" << unsigned(reg.h) << "\tl:" << unsigned(reg.l) << "\n";
        cout << "pc:" << reg.pc << "\tsp:" << reg.sp << "\n";
        cout << "Flags\n";
        cout << "z:" << ((reg.f & 0x80) != 0) << "\tn:" << ((reg.f & 0x40) != 0) << "\th:" << ((reg.f & 0x20) != 0) << "\tc:" << ((reg.f & 0x10) != 0) << "\n";
        cout << "Time passed:" << dec << t_tot << hex << endl;
    }
    void reset()
    {
        reg.reset();
        mmu.reset();
        m = t = m_tot = t_tot = 0;
        cout << hex << uppercase << "Reset\n";
        fetchExecute();
    }
    void fetchExecute()
    {
        while (1)
        {
            uint8_t op = mmu.read8(reg.pc++);
            map(op);
            m_tot += m;
            t_tot += t;
            printState();
        }
    }
    void map(uint8_t op)
    {
        switch (op)
        {
        case 0x00:
            cout << "NOP\n";
            nop();
            break;
        case 0x21:
            cout << "LD HL,NN\n";
            ldhl();
            break;
        case 0x31:
            cout << "LD SP,NN\n";
            ldsp();
            break;
        case 0x32:
            cout << "LDD [HL],A\n";
            ldHLd_a();
            break;
        case 0xAF:
            cout << "XOR A\n";
            xor_a();
            break;
        case 0xCB:
        {
            cout << "Prefix\n";
            uint8_t op = mmu.read8(reg.pc++);
            preMap(op);
            break;
        }
        default:
            cout << "Unknown Instruction 0x" << unsigned(op) << endl;
            exit(1);
        }
        t = 4 * m;
    }
    void preMap(uint8_t op)
    {
        switch (op)
        {
        case 0:
        default:
            cout << "Unknown Instruction 0xCB 0x" << unsigned(op) << endl;
            exit(1);
        }
    }
} cpu;