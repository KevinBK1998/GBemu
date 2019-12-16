#include "cpu.cpp"
void reset()
{
	gpu.reset();
	mmu.reset();
	cpu.reset();
}
void step(){
	if (cpu.reg.pc > 0x0100)
		{
			mmu.b = 0;
		}
		uint8_t op = mmu.read8(cpu.reg.pc++);
		cpu.map(op);
		cpu.m_tot += cpu.m;
		cpu.t_tot += cpu.t;
		cpu.gpuStep();
}
void frame()
{
	int nxtFrame = cpu.t_tot + 70224;
	do
	{
		step();
	} while (cpu.t_tot < nxtFrame);
}
void floop(){
	int i;
	cin>>i;
	for(;i>0;i--)
	frame();
}
int main(int argc, char *args[])
{
	char ch,name[15];
	reset();
	cin>>name;
	mmu.load(name);
	do
	{
		cin >> ch;
		if (ch == 's')
		{
			step();
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
			reset();
		}
		else if(ch=='r')
		{
			cpu.printState();
			gpu.printState();
		}
		else if(ch=='m')
		{
			mmu.dump();
		}
	} while (ch != 'e');
	return 0;
}