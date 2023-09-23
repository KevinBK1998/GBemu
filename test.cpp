#include "cpu.cpp"
// uint16_t brkpt = 0x7DA;
uint16_t brkpt = 0x150;
bool brk = true;
bool debug = false;
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
		if (debug)
			cpu.mapc(mmu.read8(cpu.reg.pc++));
		else
			cpu.map(mmu.read8(cpu.reg.pc++));
		cpu.m_tot += cpu.m;
		cpu.t_tot += cpu.t;
		cpu.gpuStep();
		cpu.timerStep();
	}
	cpu.m = cpu.t = 0;
	cpu.checkInt();
	cpu.m_tot += cpu.m;
	cpu.t_tot += cpu.t;
	if (cpu.m)
		cpu.gpuStep();
}
void sloop(int i)
{
	for (; i > 0; i--)
	{
		if (brkpt == cpu.reg.pc && brk)
		{
			cout << "BRKP@";
			brk = false;
			debug = true;
			break;
		}
		step();
	}
}
void frame()
{
	sloop(70224);
}
void floop(int i)
{
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
	char ch = 'l';
	int times = 100;
	reset(args[argc - 1]);
	floop(100);
	do
	{
		if (ch == 's')
		{
			brk = false;
			sloop(1);
		}
		else if (ch == 'S')
		{
			sloop(times);
		}
		else if (ch == 'c')
		{
			frame();
		}
		else if (ch == 'l')
		{
			floop(times);
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
			timer.printState();
		}
		else if (ch == 'm')
		{
			mmu.dump();
			mmu.dumprom();
			mmu.dumpmap();
			mmu.dumpset();
			gpu.dumpoam();
		}
		brk = true;
		cout << "IP:" << cpu.reg.pc << endl;
		cout << "Output:" << dec << unsigned(cpu.reg.b) << hex << endl;
		if (cpu.hlt)
			break;
		cout << "$:" ;
		cin >> ch;
		if (ch=='l' || ch == 'S')
		cin >> times;
	} while (ch != 'e');
	return 0;
}