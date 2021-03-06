#include "cpu.cpp"
uint16_t brkpt = 0x200;
bool brk = true;
void reset(char *name)
{
	gpu.reset();
	mmu.reset();
	cpu.reset();
	if (name[0] != '.')
		mmu.load(name);
	else
		mmu.load("ttt.gb");
}
void step()
{
	if (cpu.hlt)
		cpu.m = 1;
	else
	{
		cpu.map(mmu.read8(cpu.reg.pc++));
		cpu.m_tot += cpu.m;
		cpu.t_tot += cpu.t;
		cpu.gpuStep();
	}
	cpu.m = cpu.t = 0;
	cpu.checkInt();
	cpu.m_tot += cpu.m;
	cpu.t_tot += cpu.t;
	if (cpu.m)
		cpu.gpuStep();
	brk = true;
}
void frame()
{
	int nxtFrame = cpu.t_tot + 70224;
	do
	{
		if (brkpt == cpu.reg.pc && brk)
		{
			//cout << "BRKP:";
			brk = false;
			break;
		}
		step();

	} while (cpu.t_tot < nxtFrame);
}
void floop()
{
	int i;
	cin >> i;
	for (; i > 0; i--)
	{
		if (brkpt == cpu.reg.pc)
		{
			brk = false;
			break;
		}
		frame();
	}
}
int main(int argc, char *args[])
{
	char ch = 'c';
	reset(args[argc - 1]);
	do
	{
		//cin >> ch;
		if (ch == 's')
		{
			if (brkpt == cpu.reg.pc && brk)
			{
				cout << "BRKP:";
				brk = false;
				continue;
			}
			step();
		}
		else if (ch == 'S')
		{
			int i;
			cin >> i;
			for (; i > 0; i--)
			{
				if (brkpt == cpu.reg.pc && brk)
				{
					cout << "BRKP:";
					brk = false;
					break;
				}
				step();
			}
		}
		else if (ch == 'c')
		{
			frame();
		}
		else if (ch == 'l')
		{
			floop();
		}
		else if (ch == 'e')
		{
			reset(".");
		}
		else if (ch == 'r')
		{
			cpu.printState();
			gpu.printState();
			mmu.printState();
		}
		else if (ch == 'm')
		{
			mmu.dump();
			mmu.dumprom();
			mmu.dumpmap();
			gpu.dumpoam();
		}
	} while (ch != 'e');
	return 0;
}