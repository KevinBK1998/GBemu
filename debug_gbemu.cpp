#include "cpu.cpp"
// uint16_t brkpt = 0x90c; // TEMP.gb debugging
// uint16_t brkpt = 0x100; // bios.gb debugging
uint16_t brkpt = 0x1000; //for no debugging
bool brk = true;
bool debug = false;
void reset(char *name)
{
	gpu.reset();
	mmu.reset();
	cpu.reset();
	joyp.reset();
	if (name[0] != '.'){
		mmu.load(name);
		if(name[0] == 'c')
			brkpt = 0x77D; // cpu_instrs debugging
	}
	else
	{
		mmu.load("ttt.gb");
		// brkpt = 0x23F3; // ttt.gb debugging
	}
}
void step()
{
	if(debug)
		cout << "\nIP@" << cpu.reg.pc<<" : ";
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
			cout << "Reached Breakpoint, enter s to proceed";
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
void sleep(){
	for(int i=0;i<30000;i++)
		for(int j=0;j<32000;j++);
}
int main(int argc, char *args[])
{
	char ch = 'l';
	int times = 1000;
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
			joyp.printState();
			gpu.printState();
			mmu.printState();
			timer.printState();
		}
		else if (ch == 'm')
		{
			mmu.dump(cpu.reg.sp);
			mmu.dumpmap();
			mmu.dumpset();
			gpu.dumpoam();
		}
		brk = true;
		if (cpu.hlt){
			sleep();
			break;
		}
		cout << "\nIP@" << cpu.reg.pc << ":$ ";
		cin >> ch;
		if (ch=='l' || ch == 'S')
		cin >> times;
		// debug = true;
	} while (ch != 'e');
	return 0;
}
