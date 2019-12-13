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
    int hlt, ie;
    //0x Instructions
    void nop()
    {
        m = 1;
    }
    void ldbc()
    {
        reg.setbc(mmu.read16(reg.pc));
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
    void ldN_sp()
    {
        //LD [nnnn],SP
        mmu.write16(mmu.read16(reg.pc), reg.sp);
        reg.pc += 2;
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
        m = 1;
    }
    void ldde()
    {
        reg.setde(mmu.read16(reg.pc));
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
        reg.sethl(mmu.read16(reg.pc));
        reg.pc += 2;
        m = 3;
    }
    void ldHLi_a()
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
    void ld_HLi()
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
    void ld_HLd()
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
    //4x INstructions
    void ldb_b()
    {
        reg.b = reg.b;
        m = 1;
    }
    void ldb_c()
    {
        reg.b = reg.c;
        m = 1;
    }
    void ldb_d()
    {
        reg.b = reg.d;
        m = 1;
    }
    void ldb_e()
    {
        reg.b = reg.e;
        m = 1;
    }
    void ldb_h()
    {
        reg.b = reg.h;
        m = 1;
    }
    void ldb_l()
    {
        reg.b = reg.l;
        m = 1;
    }
    void ldb_HL()
    {
        reg.b = mmu.read8(reg.gethl());
        m = 2;
    }
    void ldb_a()
    {
        reg.b = reg.a;
        m = 1;
    }
    void ldc_b()
    {
        reg.c = reg.b;
        m = 1;
    }
    void ldc_c()
    {
        reg.c = reg.c;
        m = 1;
    }
    void ldc_d()
    {
        reg.c = reg.d;
        m = 1;
    }
    void ldc_e()
    {
        reg.c = reg.e;
        m = 1;
    }
    void ldc_h()
    {
        reg.c = reg.h;
        m = 1;
    }
    void ldc_l()
    {
        reg.c = reg.l;
        m = 1;
    }
    void ldc_HL()
    {
        reg.c = mmu.read8(reg.gethl());
        m = 2;
    }
    void ldc_a()
    {
        reg.c = reg.a;
        m = 1;
    }
    //5x Instructions
    void ldd_b()
    {
        reg.d = reg.b;
        m = 1;
    }
    void ldd_c()
    {
        reg.d = reg.c;
        m = 1;
    }
    void ldd_d()
    {
        reg.d = reg.d;
        m = 1;
    }
    void ldd_e()
    {
        reg.d = reg.e;
        m = 1;
    }
    void ldd_h()
    {
        reg.d = reg.h;
        m = 1;
    }
    void ldd_l()
    {
        reg.d = reg.l;
        m = 1;
    }
    void ldd_HL()
    {
        reg.d = mmu.read8(reg.gethl());
        m = 2;
    }
    void ldd_a()
    {
        reg.d = reg.a;
        m = 1;
    }
    void lde_b()
    {
        reg.e = reg.b;
        m = 1;
    }
    void lde_c()
    {
        reg.e = reg.c;
        m = 1;
    }
    void lde_d()
    {
        reg.e = reg.d;
        m = 1;
    }
    void lde_e()
    {
        reg.e = reg.e;
        m = 1;
    }
    void lde_h()
    {
        reg.e = reg.h;
        m = 1;
    }
    void lde_l()
    {
        reg.e = reg.l;
        m = 1;
    }
    void lde_HL()
    {
        reg.e = mmu.read8(reg.gethl());
        m = 2;
    }
    void lde_a()
    {
        reg.e = reg.a;
        m = 1;
    }
    //6x Instructions
    void ldh_b()
    {
        reg.h = reg.b;
        m = 1;
    }
    void ldh_c()
    {
        reg.h = reg.c;
        m = 1;
    }
    void ldh_d()
    {
        reg.h = reg.d;
        m = 1;
    }
    void ldh_e()
    {
        reg.h = reg.e;
        m = 1;
    }
    void ldh_h()
    {
        reg.h = reg.h;
        m = 1;
    }
    void ldh_l()
    {
        reg.h = reg.l;
        m = 1;
    }
    void ldh_HL()
    {
        reg.h = mmu.read8(reg.gethl());
        m = 2;
    }
    void ldh_a()
    {
        reg.h = reg.a;
        m = 1;
    }
    void ldl_b()
    {
        reg.l = reg.b;
        m = 1;
    }
    void ldl_c()
    {
        reg.l = reg.c;
        m = 1;
    }
    void ldl_d()
    {
        reg.l = reg.d;
        m = 1;
    }
    void ldl_e()
    {
        reg.l = reg.e;
        m = 1;
    }
    void ldl_h()
    {
        reg.l = reg.h;
        m = 1;
    }
    void ldl_l()
    {
        reg.l = reg.l;
        m = 1;
    }
    void ldl_HL()
    {
        reg.l = mmu.read8(reg.gethl());
        m = 2;
    }
    void ldl_a()
    {
        reg.l = reg.a;
        m = 1;
    }
    //7x Instructions
    void ldHL_b()
    {
        mmu.write8(reg.gethl(), reg.b);
        m = 2;
    }
    void ldHL_c()
    {
        mmu.write8(reg.gethl(), reg.c);
        m = 2;
    }
    void ldHL_d()
    {
        mmu.write8(reg.gethl(), reg.d);
        m = 2;
    }
    void ldHL_e()
    {
        mmu.write8(reg.gethl(), reg.e);
        m = 2;
    }
    void ldHL_h()
    {
        mmu.write8(reg.gethl(), reg.h);
        m = 2;
    }
    void ldHL_l()
    {
        mmu.write8(reg.gethl(), reg.l);
        m = 2;
    }
    void halt()
    {
        hlt = 1;
        m = 1;
    }
    void ldHL_a()
    {
        mmu.write8(reg.gethl(), reg.a);
        m = 2;
    }
    void lda_b()
    {
        reg.a = reg.b;
        m = 1;
    }
    void lda_c()
    {
        reg.a = reg.c;
        m = 1;
    }
    void lda_d()
    {
        reg.a = reg.d;
        m = 1;
    }
    void lda_e()
    {
        reg.a = reg.e;
        m = 1;
    }
    void lda_h()
    {
        reg.a = reg.h;
        m = 1;
    }
    void lda_l()
    {
        reg.a = reg.l;
        m = 1;
    }
    void lda_HL()
    {
        reg.a = mmu.read8(reg.gethl());
        m = 2;
    }
    void lda_a()
    {
        reg.a = reg.a;
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
    void sub_b()
    {
        int result = reg.a - reg.b;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.b & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sub_c()
    {
        int result = reg.a - reg.c;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.c & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sub_d()
    {
        int result = reg.a - reg.d;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.d & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sub_e()
    {
        int result = reg.a - reg.e;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.e & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sub_h()
    {
        int result = reg.a - reg.h;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.h & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sub_l()
    {
        int result = reg.a - reg.l;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.l & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sub_HL()
    {
        //sub A,[HL]
        int result = reg.a - mmu.read8(reg.gethl());
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (mmu.read8(reg.gethl()) & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 2;
    }
    void sub_a()
    {
        int result = reg.a - reg.a;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - (reg.a & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_b()
    {
        int result = reg.a - (reg.b + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.b & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_c()
    {
        int result = reg.a - (reg.c + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.c & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_d()
    {
        int result = reg.a - (reg.d + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.d & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_e()
    {
        int result = reg.a - (reg.e + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.e & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_h()
    {
        int result = reg.a - (reg.h + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.h & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_l()
    {
        int result = reg.a - (reg.l + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.l & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    void sbc_HL()
    {
        //sbc A,[HL]
        int result = reg.a - (mmu.read8(reg.gethl()) + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((mmu.read8(reg.gethl()) & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 2;
    }
    void sbc_a()
    {
        int result = reg.a - (reg.a + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((reg.a & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        m = 1;
    }
    //Ax Instructions
    void and_b()
    {
        uint8_t res = reg.a & reg.b;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void and_c()
    {
        uint8_t res = reg.a & reg.c;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void and_d()
    {
        uint8_t res = reg.a & reg.d;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void and_e()
    {
        uint8_t res = reg.a & reg.e;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void and_h()
    {
        uint8_t res = reg.a & reg.h;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void and_l()
    {
        uint8_t res = reg.a & reg.l;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void and_HL()
    {
        uint8_t res = reg.a & mmu.read8(reg.gethl());
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 2;
    }
    void and_a()
    {
        uint8_t res = reg.a & reg.a;
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_b()
    {
        uint8_t res = reg.a ^ reg.b;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_c()
    {
        uint8_t res = reg.a ^ reg.c;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_d()
    {
        uint8_t res = reg.a ^ reg.d;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_e()
    {
        uint8_t res = reg.a ^ reg.e;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_h()
    {
        uint8_t res = reg.a ^ reg.h;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_l()
    {
        uint8_t res = reg.a ^ reg.l;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void xor_HL()
    {
        uint8_t res = reg.a ^ mmu.read8(reg.gethl());
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 2;
    }
    void xor_a()
    {
        uint8_t res = reg.a ^ reg.a;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    //Bx Instructions
    void or_b()
    {
        uint8_t res = reg.a | reg.b;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void or_c()
    {
        uint8_t res = reg.a | reg.c;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void or_d()
    {
        uint8_t res = reg.a | reg.d;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void or_e()
    {
        uint8_t res = reg.a | reg.e;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void or_h()
    {
        uint8_t res = reg.a | reg.h;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void or_l()
    {
        uint8_t res = reg.a | reg.l;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void or_HL()
    {
        uint8_t res = reg.a | mmu.read8(reg.gethl());
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 2;
    }
    void or_a()
    {
        uint8_t res = reg.a | reg.a;
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        m = 1;
    }
    void cp_b()
    {
        int res = reg.a - reg.b;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.b & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    void cp_c()
    {
        int res = reg.a - reg.c;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.c & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    void cp_d()
    {
        int res = reg.a - reg.d;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.d & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    void cp_e()
    {
        int res = reg.a - reg.e;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.e & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    void cp_h()
    {
        int res = reg.a - reg.h;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.h & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    void cp_l()
    {
        int res = reg.a - reg.l;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.l & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    void cp_HL()
    {
        int res = reg.a - mmu.read8(reg.gethl());
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (mmu.read8(reg.gethl()) & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 2;
    }
    void cp_a()
    {
        int res = reg.a - reg.a;
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (reg.a & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        m = 1;
    }
    //Cx Instructions
    void retnz()
    {
        m = 1;
        if (!(reg.f & 0x80))
        {
            reg.pc = mmu.read16(reg.sp);
            reg.sp += 2;
            m += 2;
        }
    }
    void popbc()
    {
        reg.setbc(mmu.read16(reg.sp));
        reg.sp += 2;
        m = 3;
    }
    void jpnz_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (!(reg.f & 0x80))
        {
            reg.pc = nn;
            m++;
        }
    }
    void jp_nn()
    {
        reg.pc = mmu.read16(reg.pc);
        m = 3;
    }
    void callnz_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (!(reg.f & 0x80))
        {
            reg.sp -= 2;
            mmu.write16(reg.sp, reg.pc);
            reg.pc = nn;
            m += 2;
        }
    }
    void pushbc()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.getbc());
        m = 3;
    }
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
    void rst_0()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0000;
        m = 3;
    }
    void retz()
    {
        m = 1;
        if (reg.f & 0x80)
        {
            reg.pc = mmu.read16(reg.sp);
            reg.sp += 2;
            m += 2;
        }
    }
    void ret()
    {
        reg.pc = mmu.read16(reg.sp);
        reg.sp += 2;
        m = 3;
    }
    void jpz_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (reg.f & 0x80)
        {
            reg.pc = nn;
            m++;
        }
    }
    void CB()
    {
        uint8_t op = mmu.read8(reg.pc++);
        preMap(op);
    }
    void callz_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (reg.f & 0x80)
        {
            reg.sp -= 2;
            mmu.write16(reg.sp, reg.pc);
            reg.pc = nn;
            m += 2;
        }
    }
    void call_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = nn;
        m = 5;
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
    void rst_8()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0008;
        m = 3;
    }
    //Dx Instructions
    void retnc()
    {
        m = 1;
        if (!(reg.f & 0x10))
        {
            reg.pc = mmu.read16(reg.sp);
            reg.sp += 2;
            m += 2;
        }
    }
    void popde()
    {
        reg.setde(mmu.read16(reg.sp));
        reg.sp += 2;
        m = 3;
    }
    void jpnc_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (!(reg.f & 0x10))
        {
            reg.pc = nn;
            m++;
        }
    }
    void ND()
    {
        //0xD3 not defined
        cout << "Exit()\n";
        exit(1);
    }
    void callnc_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (!(reg.f & 0x10))
        {
            reg.sp -= 2;
            mmu.write16(reg.sp, reg.pc);
            reg.pc = nn;
            m += 2;
        }
    }
    void pushde()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.getde());
        m = 3;
    }
    void sub_n()
    {
        uint8_t n = mmu.read8(reg.pc);
        int result = reg.a - n;
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (n & 0xF) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        reg.pc++;
        m = 2;
    }
    void rst_10()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0010;
        m = 3;
    }
    void retc()
    {
        m = 1;
        if (reg.f & 0x10)
        {
            reg.pc = mmu.read16(reg.sp);
            reg.sp += 2;
            m += 2;
        }
    }
    void reti()
    {
        reg.pc = mmu.read16(reg.sp);
        reg.sp += 2;
        ie = 1;
        m = 3;
    }
    void jpc_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (reg.f & 0x10)
        {
            reg.pc = nn;
            m++;
        }
    }
    //0xDB not defined
    void callc_nn()
    {
        uint16_t nn = mmu.read16(reg.pc);
        reg.pc += 2;
        m = 3; //doubt
        if (reg.f & 0x10)
        {
            reg.sp -= 2;
            mmu.write16(reg.sp, reg.pc);
            reg.pc = nn;
            m += 2;
        }
    }
    //0xDD not defined
    void sbc_n()
    {
        uint8_t n = mmu.read8(reg.pc);
        int result = reg.a - (n + ((reg.f & 0x10) != 0));
        reg.f = 0;
        if (!(result & 0xFF))
            reg.f |= 0x80;
        reg.f |= 0x40;
        if ((reg.a & 0xF) - ((n & 0xF) + ((reg.f & 0x10) != 0)) < 0)
            reg.f |= 0x20;
        if (result < 0)
            reg.f |= 0x10;
        reg.a = result;
        reg.pc++;
        m = 2;
    }
    void rst_18()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0018;
        m = 3;
    }
    //Ex Instructions
    void ldHN_a()
    {
        mmu.write8(0xFF00 + mmu.read8(reg.pc), reg.a);
        reg.pc++;
        m = 3;
    }
    void pophl()
    {
        reg.sethl(mmu.read16(reg.sp));
        reg.sp += 2;
        m = 3;
    }
    void ldHC_a()
    {
        mmu.write8(0xFF00 + reg.c, reg.a);
        m = 2;
    }
    //0xE3 not defined
    //0xE4 not defined
    void pushhl()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.gethl());
        m = 3;
    }
    void and_n()
    {
        uint8_t res = reg.a & mmu.read8(reg.pc);
        reg.f = 0x20;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        reg.pc++;
        m = 2;
    }
    void rst_20()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0020;
        m = 3;
    }
    void addsp_n()
    {
        int8_t n = mmu.read8(reg.pc);
        reg.pc++;
        reg.sp += n;
        m = 1;
    }
    void jp_HL()
    {
        reg.pc = mmu.read16(reg.gethl());
        m = 1;
    }
    void ldNN_a()
    {
        mmu.write8(mmu.read16(reg.pc), reg.a);
        reg.pc += 2;
        m = 4;
    }
    //0xEB not defined
    //0xEC not defined
    //0xED not defined
    void xor_n()
    {
        uint8_t res = reg.a ^ mmu.read8(reg.pc);
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        reg.pc++;
        m = 2;
    }
    void rst_28()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0028;
        m = 3;
    }
    //Fx Instructions
    void lda_HN()
    {
        reg.a = mmu.read8(0xFF00 + mmu.read8(reg.pc));
        reg.pc++;
        m = 3;
    }
    void popaf()
    {
        reg.setaf(mmu.read16(reg.sp));
        reg.sp += 2;
        m = 3;
    }
    //0xF2 not defined
    void di()
    {
        ie = 0;
        m = 1;
    }
    //0xF4 not defined
    void pushaf()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.getaf());
        m = 3;
    }
    void or_n()
    {
        uint8_t res = reg.a | mmu.read8(reg.pc);
        reg.f = 0;
        if (!res)
            reg.f |= 0x80;
        reg.a = res;
        reg.pc++;
        m = 2;
    }
    void rst_30()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0030;
        m = 3;
    }
    void ldhl_sp_n()
    {
        int8_t n = mmu.read8(reg.pc);
        uint16_t res = reg.sp + n;
        reg.sethl(res);
        reg.pc++;
        reg.f = 0;
        m = 2;
    }
    void ldsp_hl()
    {
        reg.sp = reg.gethl();
        m = 1;
    }
    void ld_NN()
    {
        reg.a = mmu.read8(mmu.read16(reg.pc));
        reg.pc += 2;
        m = 4;
    }
    void ei()
    {
        ie = 1;
        m = 1;
    }
    //0xFC not defined
    //0xFD not defined
    void cp_n()
    {
        int res = reg.a - mmu.read8(reg.pc);
        reg.f = 0x40;
        if (!(res & 0xFF))
            reg.f |= 0x80;
        if ((reg.a & 0xF) - (mmu.read8(reg.pc) & 0xF) < 0)
            reg.f |= 0x20;
        if (res < 0)
            reg.f |= 0x10;
        reg.pc++;
        m = 2;
    }
    void rst_38()
    {
        reg.sp -= 2;
        mmu.write16(reg.sp, reg.pc + 2);
        reg.pc = 0x0038;
        m = 3;
    }
    //Implementation and Mapping
    void printState()
    {
        cout << "a:" << hex << uppercase << unsigned(reg.a) << "\tf:" << unsigned(reg.f) << "\n";
        cout << "b:" << unsigned(reg.b) << "\tc:" << unsigned(reg.c) << "\n";
        cout << "d:" << unsigned(reg.d) << "\te:" << unsigned(reg.e) << "\n";
        cout << "h:" << unsigned(reg.h) << "\tl:" << unsigned(reg.l) << "\n";
        cout << "pc:" << reg.pc << "\tsp:" << reg.sp << "\n";
        cout << "Flags\n";
        cout << "z:" << ((reg.f & 0x80) != 0) << "\tn:" << ((reg.f & 0x40) != 0) << "\th:" << ((reg.f & 0x20) != 0) << "\tc:" << ((reg.f & 0x10) != 0) << "\n";
        cout << "Time passed:" << dec << t_tot / 1000 << " ms" << endl;
        cout << "Cycles passed:" << m_tot << hex << endl;
    }
    void reset()
    {
        reg.reset();
        mmu.reset();
        gpu.reset();
        m = t = m_tot = t_tot = hlt = 0;
        ie = 1;
        cout << hex << uppercase << "Reset\n";
        fetchExecute();
    }
    void gpuStep()
    {
        gpu.clk += t;
        switch (gpu.mode)
        {
        case 0:
            if (gpu.clk >= 204)
            { //if hblank done go to oam
                gpu.clk = 0;
                gpu.line++;
                if (gpu.line == 143)
                { //if last line go to vblank after render screen
                    gpu.mode = 1;
                    //gpu.screen();
                }
                else
                {
                    gpu.mode = 2;
                }
            }
            break;
        case 1:
            if (gpu.clk >= 456)
            { //wait for 10 lines
                gpu.clk = 0;
                gpu.line++;
                if (gpu.line == 153)
                {  //if vblank done go to oam(repeat)
                    gpu.mode = 2;
                    gpu.line = 0;
                }
            }
            break;
        case 2:
            if (gpu.clk >= 80)
            { //if oam done go to vram
                gpu.mode = 3;
                gpu.clk = 0;
            }
            break;
        case 3:
            if (gpu.clk >= 172)
            { //if vram done scanline and enter hblank
                gpu.mode = 0;
                gpu.clk = 0;
                //gpu.scanline();
            }
            break;
        default:
            cout << "gpu time error\n";
        }
    }
    void fetchExecute()
    {
        while (1)
        {
            if (reg.pc > 0x0100)
            {
                mmu.b = 0;
                exit(1);
            }
            uint8_t op = mmu.read8(reg.pc++);
            map(op);
            m_tot += m;
            t_tot += t;
            gpuStep();
            printState();
            if(t_tot>500000)break;
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
        case 0x01:
            cout << "LD BC,NN\n";
            ldbc();
            break;
        case 0x02:
            cout << "LD [BC],A\n";
            ldBC_a();
            break;
        case 0x03:
            cout << "INC BC\n";
            inc_bc();
            break;
        case 0x04:
            cout << "INC B\n";
            inc_b();
            break;
        case 0x05:
            cout << "DEC B\n";
            dec_b();
            break;
        case 0x06:
            cout << "LD B,N\n";
            ldb_n();
            break;
        case 0x07:
            cout << "RLC A\n";
            rlc_a();
            break;
        case 0x08:
            cout << "LD [NN],SP\n";
            ldN_sp();
            break;
        case 0x09:
            cout << "ADD HL,BC\n";
            addhl_bc();
            break;
        case 0x0A:
            cout << "LD A,[BC]\n";
            ld_BC();
            break;
        case 0x0B:
            cout << "DEC BC\n";
            dec_bc();
            break;
        case 0x0C:
            cout << "INC C\n";
            inc_c();
            break;
        case 0x0D:
            cout << "DEC C\n";
            dec_c();
            break;
        case 0x0E:
            cout << "LD C,N\n";
            ldc_n();
            break;
        case 0x0F:
            cout << "RRC A\n";
            rrc_a();
            break;
        case 0x10:
            cout << "STOP\n";
            //stop();
            ND();
            break;
        case 0x11:
            cout << "LD DE,NN\n";
            ldde();
            break;
        case 0x12:
            cout << "LD [DE],A\n";
            ldDE_a();
            break;
        case 0x13:
            cout << "INC DE\n";
            inc_de();
            break;
        case 0x14:
            cout << "INC D\n";
            inc_d();
            break;
        case 0x15:
            cout << "DEC D\n";
            dec_d();
            break;
        case 0x16:
            cout << "LD D,N\n";
            ldd_n();
            break;
        case 0x17:
            cout << "RL A\n";
            rl_a();
            break;
        case 0x18:
            cout << "JR N\n";
            jr_n();
            break;
        case 0x19:
            cout << "ADD HL,DE\n";
            addhl_de();
            break;
        case 0x1A:
            cout << "LD A,[DE]\n";
            ld_DE();
            break;
        case 0x1B:
            cout << "DEC DE\n";
            dec_de();
            break;
        case 0x1C:
            cout << "INC E\n";
            inc_e();
            break;
        case 0x1D:
            cout << "DEC E\n";
            dec_e();
            break;
        case 0x1E:
            cout << "LD E,N\n";
            lde_n();
            break;
        case 0x1F:
            cout << "RR A\n";
            rr_a();
            break;
        case 0x20:
            cout << "JR NZ,N\n";
            jrnz_n();
            break;
        case 0x21:
            cout << "LD HL,NN\n";
            ldhl();
            break;
        case 0x22:
            cout << "LDI [HL],A\n";
            ldHLi_a();
            break;
        case 0x23:
            cout << "INC HL\n";
            inc_hl();
            break;
        case 0x24:
            cout << "INC H\n";
            inc_h();
            break;
        case 0x25:
            cout << "DEC H\n";
            dec_h();
            break;
        case 0x26:
            cout << "LD H,N\n";
            ldh_n();
            break;
        case 0x27:
            cout << "DAA\n";
            daa();
            break;
        case 0x28:
            cout << "JR Z,N\n";
            jrz_n();
            break;
        case 0x29:
            cout << "ADD HL,HL\n";
            addhl_hl();
            break;
        case 0x2A:
            cout << "LDI A,[HL]\n";
            ld_HLi();
            break;
        case 0x2B:
            cout << "DEC HL\n";
            dec_hl();
            break;
        case 0x2C:
            cout << "INC L\n";
            inc_l();
            break;
        case 0x2D:
            cout << "DEC L\n";
            dec_l();
            break;
        case 0x2E:
            cout << "LD L,N\n";
            ldl_n();
            break;
        case 0x2F:
            cout << "CPL\n";
            cpl();
            break;
        case 0x30:
            cout << "JR NC,N\n";
            jrnc_n();
            break;
        case 0x31:
            cout << "LD SP,NN\n";
            ldsp();
            break;
        case 0x32:
            cout << "LDD [HL],A\n";
            ldHLd_a();
            break;
        case 0x33:
            cout << "INC SP\n";
            inc_sp();
            break;
        case 0x34:
            cout << "INC [HL]\n";
            inc_HL();
            break;
        case 0x35:
            cout << "DEC [HL]\n";
            dec_HL();
            break;
        case 0x36:
            cout << "LD [HL],N\n";
            ldHL_n();
            break;
        case 0x37:
            cout << "SCF\n";
            scf();
            break;
        case 0x38:
            cout << "JR C,N\n";
            jrc_n();
            break;
        case 0x39:
            cout << "ADD HL,SP\n";
            addhl_sp();
            break;
        case 0x3A:
            cout << "LDD A,[HL]\n";
            ld_HLd();
            break;
        case 0x3B:
            cout << "DEC SP\n";
            dec_sp();
            break;
        case 0x3C:
            cout << "INC A\n";
            inc_a();
            break;
        case 0x3D:
            cout << "DEC A\n";
            dec_a();
            break;
        case 0x3E:
            cout << "LD A,N\n";
            ld_n();
            break;
        case 0x3F:
            cout << "CCF\n";
            ccf();
            break;
        case 0x40:
            cout << "LD B,B\n";
            ldb_b();
            break;
        case 0x41:
            cout << "LD B,C\n";
            ldb_c();
            break;
        case 0x42:
            cout << "LD B,D\n";
            ldb_d();
            break;
        case 0x43:
            cout << "LD B,E\n";
            ldb_e();
            break;
        case 0x44:
            cout << "LD B,H\n";
            ldb_h();
            break;
        case 0x45:
            cout << "LD B,L\n";
            ldb_l();
            break;
        case 0x46:
            cout << "LD B,[HL]\n";
            ldb_HL();
            break;
        case 0x47:
            cout << "LD B,A\n";
            ldb_a();
            break;
        case 0x48:
            cout << "LD C,B\n";
            ldc_b();
            break;
        case 0x49:
            cout << "LD C,C\n";
            ldc_c();
            break;
        case 0x4A:
            cout << "LD C,D\n";
            ldc_d();
            break;
        case 0x4B:
            cout << "LD C,E\n";
            ldc_e();
            break;
        case 0x4C:
            cout << "LD C,H\n";
            ldc_h();
            break;
        case 0x4D:
            cout << "LD C,L\n";
            ldc_l();
            break;
        case 0x4E:
            cout << "LD C,[HL]\n";
            ldc_HL();
            break;
        case 0x4F:
            cout << "LD C,A\n";
            ldc_a();
            break;
        case 0x50:
            cout << "LD D,B\n";
            ldd_b();
            break;
        case 0x51:
            cout << "LD D,C\n";
            ldd_c();
            break;
        case 0x52:
            cout << "LD D,D\n";
            ldd_d();
            break;
        case 0x53:
            cout << "LD D,E\n";
            ldd_e();
            break;
        case 0x54:
            cout << "LD D,H\n";
            ldd_h();
            break;
        case 0x55:
            cout << "LD D,L\n";
            ldd_l();
            break;
        case 0x56:
            cout << "LD D,[HL]\n";
            ldd_HL();
            break;
        case 0x57:
            cout << "LD D,A\n";
            ldd_a();
            break;
        case 0x58:
            cout << "LD E,B\n";
            lde_b();
            break;
        case 0x59:
            cout << "LD E,C\n";
            lde_c();
            break;
        case 0x5A:
            cout << "LD E,D\n";
            lde_d();
            break;
        case 0x5B:
            cout << "LD E,E\n";
            lde_e();
            break;
        case 0x5C:
            cout << "LD E,H\n";
            lde_h();
            break;
        case 0x5D:
            cout << "LD E,L\n";
            lde_l();
            break;
        case 0x5E:
            cout << "LD E,[HL]\n";
            lde_HL();
            break;
        case 0x5F:
            cout << "LD E,A\n";
            lde_a();
            break;
        case 0x60:
            cout << "LD H,B\n";
            ldh_b();
            break;
        case 0x61:
            cout << "LD H,C\n";
            ldh_c();
            break;
        case 0x62:
            cout << "LD H,D\n";
            ldh_d();
            break;
        case 0x63:
            cout << "LD H,E\n";
            ldh_e();
            break;
        case 0x64:
            cout << "LD H,H\n";
            ldh_h();
            break;
        case 0x65:
            cout << "LD H,L\n";
            ldh_l();
            break;
        case 0x66:
            cout << "LD H,[HL]\n";
            ldh_HL();
            break;
        case 0x67:
            cout << "LD H,A\n";
            ldh_a();
            break;
        case 0x68:
            cout << "LD L,B\n";
            ldl_b();
            break;
        case 0x69:
            cout << "LD L,C\n";
            ldl_c();
            break;
        case 0x6A:
            cout << "LD L,D\n";
            ldl_d();
            break;
        case 0x6B:
            cout << "LD L,E\n";
            ldl_e();
            break;
        case 0x6C:
            cout << "LD L,H\n";
            ldl_h();
            break;
        case 0x6D:
            cout << "LD L,L\n";
            ldl_l();
            break;
        case 0x6E:
            cout << "LD L,[HL]\n";
            ldl_HL();
            break;
        case 0x6F:
            cout << "LD L,A\n";
            ldl_a();
            break;
        case 0x70:
            cout << "LD [HL],B\n";
            ldHL_b();
            break;
        case 0x71:
            cout << "LD [HL],C\n";
            ldHL_c();
            break;
        case 0x72:
            cout << "LD [HL],D\n";
            ldHL_d();
            break;
        case 0x73:
            cout << "LD [HL],E\n";
            ldHL_e();
            break;
        case 0x74:
            cout << "LD [HL],H\n";
            ldHL_h();
            break;
        case 0x75:
            cout << "LD [HL],L\n";
            ldHL_l();
            break;
        case 0x76:
            cout << "HALT\n";
            halt();
            break;
        case 0x77:
            cout << "LD [HL],A\n";
            ldHL_a();
            break;
        case 0x78:
            cout << "LD A,B\n";
            lda_b();
            break;
        case 0x79:
            cout << "LD A,C\n";
            lda_c();
            break;
        case 0x7A:
            cout << "LD A,D\n";
            lda_d();
            break;
        case 0x7B:
            cout << "LD A,E\n";
            lda_e();
            break;
        case 0x7C:
            cout << "LD A,H\n";
            lda_h();
            break;
        case 0x7D:
            cout << "LD A,L\n";
            lda_l();
            break;
        case 0x7E:
            cout << "LD A,[HL]\n";
            lda_HL();
            break;
        case 0x7F:
            cout << "LD A,A\n";
            lda_a();
            break;
        case 0x80:
            cout << "ADD A,B\n";
            add_b();
            break;
        case 0x81:
            cout << "ADD A,C\n";
            add_c();
            break;
        case 0x82:
            cout << "ADD A,D\n";
            add_d();
            break;
        case 0x83:
            cout << "ADD A,E\n";
            add_e();
            break;
        case 0x84:
            cout << "ADD A,H\n";
            add_h();
            break;
        case 0x85:
            cout << "ADD A,L\n";
            add_l();
            break;
        case 0x86:
            cout << "ADD A,[HL]\n";
            add_HL();
            break;
        case 0x87:
            cout << "ADD A,A\n";
            add_a();
            break;
        case 0x88:
            cout << "ADC A,B\n";
            adc_b();
            break;
        case 0x89:
            cout << "ADC A,C\n";
            adc_c();
            break;
        case 0x8A:
            cout << "ADC A,D\n";
            adc_d();
            break;
        case 0x8B:
            cout << "ADC A,E\n";
            adc_e();
            break;
        case 0x8C:
            cout << "ADC A,H\n";
            adc_h();
            break;
        case 0x8D:
            cout << "ADC A,L\n";
            adc_l();
            break;
        case 0x8E:
            cout << "ADC A,[HL]\n";
            adc_HL();
            break;
        case 0x8F:
            cout << "ADC A,A\n";
            adc_a();
            break;
        case 0x90:
            cout << "SUB A,B\n";
            sub_b();
            break;
        case 0x91:
            cout << "SUB A,C\n";
            sub_c();
            break;
        case 0x92:
            cout << "SUB A,D\n";
            sub_d();
            break;
        case 0x93:
            cout << "SUB A,E\n";
            sub_e();
            break;
        case 0x94:
            cout << "SUB A,H\n";
            sub_h();
            break;
        case 0x95:
            cout << "SUB A,L\n";
            sub_l();
            break;
        case 0x96:
            cout << "SUB A,[HL]\n";
            sub_HL();
            break;
        case 0x97:
            cout << "SUB A,A\n";
            sub_a();
            break;
        case 0x98:
            cout << "SBC A,B\n";
            sbc_b();
            break;
        case 0x99:
            cout << "SBC A,C\n";
            sbc_c();
            break;
        case 0x9A:
            cout << "SBC A,D\n";
            sbc_d();
            break;
        case 0x9B:
            cout << "SBC A,E\n";
            sbc_e();
            break;
        case 0x9C:
            cout << "SBC A,H\n";
            sbc_h();
            break;
        case 0x9D:
            cout << "SBC A,L\n";
            sbc_l();
            break;
        case 0x9E:
            cout << "SBC A,[HL]\n";
            sbc_HL();
            break;
        case 0x9F:
            cout << "SBC A,A\n";
            sbc_a();
            break;
        case 0xA0:
            cout << "AND B\n";
            and_b();
            break;
        case 0xA1:
            cout << "AND C\n";
            and_c();
            break;
        case 0xA2:
            cout << "AND D\n";
            and_d();
            break;
        case 0xA3:
            cout << "AND E\n";
            and_e();
            break;
        case 0xA4:
            cout << "AND H\n";
            and_h();
            break;
        case 0xA5:
            cout << "AND L\n";
            and_l();
            break;
        case 0xA6:
            cout << "AND [HL]\n";
            and_HL();
            break;
        case 0xA7:
            cout << "AND A\n";
            and_a();
            break;
        case 0xA8:
            cout << "ADC A,B\n";
            xor_b();
            break;
        case 0xA9:
            cout << "ADC A,C\n";
            xor_c();
            break;
        case 0xAA:
            cout << "ADC A,D\n";
            xor_d();
            break;
        case 0xAB:
            cout << "ADC A,E\n";
            xor_e();
            break;
        case 0xAC:
            cout << "ADC A,H\n";
            xor_h();
            break;
        case 0xAD:
            cout << "ADC A,L\n";
            xor_l();
            break;
        case 0xAE:
            cout << "ADC A,[HL]\n";
            xor_HL();
            break;
        case 0xAF:
            cout << "ADC A,A\n";
            xor_a();
            break;
        case 0xB0:
            cout << "OR B\n";
            or_b();
            break;
        case 0xB1:
            cout << "OR C\n";
            or_c();
            break;
        case 0xB2:
            cout << "OR D\n";
            or_d();
            break;
        case 0xB3:
            cout << "OR E\n";
            or_e();
            break;
        case 0xB4:
            cout << "OR H\n";
            or_h();
            break;
        case 0xB5:
            cout << "OR L\n";
            or_l();
            break;
        case 0xB6:
            cout << "OR [HL]\n";
            or_HL();
            break;
        case 0xB7:
            cout << "OR A\n";
            or_a();
            break;
        case 0xB8:
            cout << "ADC A,B\n";
            cp_b();
            break;
        case 0xB9:
            cout << "ADC A,C\n";
            cp_c();
            break;
        case 0xBA:
            cout << "ADC A,D\n";
            cp_d();
            break;
        case 0xBB:
            cout << "ADC A,E\n";
            cp_e();
            break;
        case 0xBC:
            cout << "ADC A,H\n";
            cp_h();
            break;
        case 0xBD:
            cout << "ADC A,L\n";
            cp_l();
            break;
        case 0xBE:
            cout << "ADC A,[HL]\n";
            cp_HL();
            break;
        case 0xBF:
            cout << "ADC A,A\n";
            cp_a();
            break;
        case 0xC0:
            cout << "RET NZ\n";
            retnz();
            break;
        case 0xC1:
            cout << "POP BC\n";
            popbc();
            break;
        case 0xC2:
            cout << "JP NZ,NN\n";
            jpnz_nn();
            break;
        case 0xC3:
            cout << "JP NN\n";
            jp_nn();
            break;
        case 0xC4:
            cout << "CALL NZ,NN\n";
            callnz_nn();
            break;
        case 0xC5:
            cout << "PUSH BC\n";
            pushbc();
            break;
        case 0xC6:
            cout << "ADD A,N\n";
            add_n();
            break;
        case 0xC7:
            cout << "RST 0x00\n";
            rst_0();
            break;
        case 0xC8:
            cout << "RET Z\n";
            retz();
            break;
        case 0xC9:
            cout << "RET\n";
            ret();
            break;
        case 0xCA:
            cout << "JP Z,NN\n";
            jpz_nn();
            break;
        case 0xCB:
            cout << "0xCB:\n";
            CB();
            break;
        case 0xCC:
            cout << "CALL Z,NN\n";
            callz_nn();
            break;
        case 0xCD:
            cout << "CALL NN\n";
            call_nn();
            break;
        case 0xCE:
            cout << "ADC A,N\n";
            adc_n();
            break;
        case 0xCF:
            cout << "RST 0x08\n";
            rst_8();
            break;
        case 0xD0:
            cout << "RET NC\n";
            retnc();
            break;
        case 0xD1:
            cout << "POP DE\n";
            popde();
            break;
        case 0xD2:
            cout << "JP NC,NN\n";
            jpnc_nn();
            break;
        //D3 not defined
        case 0xD4:
            cout << "CALL NC,NN\n";
            callnc_nn();
            break;
        case 0xD5:
            cout << "PUSH DE\n";
            pushde();
            break;
        case 0xD6:
            cout << "SUB A,N\n";
            sub_n();
            break;
        case 0xD7:
            cout << "RST 0x10\n";
            rst_10();
            break;
        case 0xD8:
            cout << "RET C\n";
            retc();
            break;
        case 0xD9:
            cout << "RETI\n";
            reti();
            break;
        case 0xDA:
            cout << "JP C,NN\n";
            jpc_nn();
            break;
            //DB not defined
        case 0xDC:
            cout << "CALL C,NN\n";
            callc_nn();
            break;
            //DD not defined
        case 0xDE:
            cout << "SBC A,N\n";
            sbc_n();
            break;
        case 0xDF:
            cout << "RST 0x18\n";
            rst_18();
            break;
        case 0xE0:
            cout << "LD [HN],A\n";
            ldHN_a();
            break;
        case 0xE1:
            cout << "POP HL\n";
            pophl();
            break;
        case 0xE2:
            cout << "LD [HC],A\n";
            ldHC_a();
            break;
            //E3 not defined
            //E4 not defined
        case 0xE5:
            cout << "PUSH HL\n";
            pushhl();
            break;
        case 0xE6:
            cout << "AND A,N\n";
            and_n();
            break;
        case 0xE7:
            cout << "RST 0x20\n";
            ldHN_a();
            break;
        case 0xE8:
            cout << "ADD SP,n(signed)\n";
            addsp_n();
            break;
        case 0xE9:
            cout << "JP [HL]\n";
            jp_HL();
            break;
        case 0xEA:
            cout << "LD [NN],A\n";
            ldNN_a();
            break;
            //EB not defined
            //EC not defined
            //ED not defined
        case 0xEE:
            cout << "XOR A,N\n";
            xor_n();
            break;
        case 0xEF:
            cout << "RST 0x28\n";
            rst_28();
            break;
        case 0xF0:
            cout << "LD A,[HN]\n";
            lda_HN();
            break;
        case 0xF1:
            cout << "POP AF\n";
            popaf();
            break;
            //F2 not defined
        case 0xF3:
            cout << "DI\n";
            di();
            break;
            //F4 not defined
        case 0xF5:
            cout << "PUSH AF\n";
            pushaf();
            break;
        case 0xF6:
            cout << "OR A,N\n";
            or_n();
            break;
        case 0xF7:
            cout << "RST 0x30\n";
            rst_30();
            break;
        case 0xF8:
            cout << "LDHL SP,n(signed)\n";
            ldhl_sp_n();
            break;
        case 0xF9:
            cout << "LD SP,HL\n";
            ldsp_hl();
            break;
        case 0xFA:
            cout << "LD A,[NN]\n";
            ld_NN();
            break;
        case 0xFB:
            cout << "EI\n";
            ei();
            break;
        case 0xFE:
            cout << "CP A,N\n";
            cp_n();
            break;
        case 0xFF:
            cout << "RST 0x38\n";
            rst_38();
            break;
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
        case 0x11:
            cout << "RL C\n";
            rl_c();
            break;
        case 0x7C:
            cout << "BIT 7,H\n";
            bit7_h();
            break;
        default:
            cout << "Unknown Instruction 0xCB 0x" << unsigned(op) << endl;
            exit(1);
        }
    }
    //1x Instructions
    void rl_c()
    {
        int c = ((reg.f & 0x10) != 0);
        reg.f = 0;
        reg.f |= ((reg.c & 0x80) >> 3);
        reg.c <<= 1;
        reg.c += c;
        if (!reg.c)
            reg.f |= 0x80;
        m = 1;
    }
    //7x Instructions
    void bit7_h()
    {
        reg.f &= 0x10;
        reg.f |= 0x20;
        if (!(reg.h >> 7))
            reg.f |= 0x80;
        m = 2;
    }
} cpu;