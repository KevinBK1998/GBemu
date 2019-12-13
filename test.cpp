#include "gpu.cpp"
int main(int argc, char *args[])
{
	gpu.reset();
    gpu.loop();
	//Free resources and close SDL
	gpu.close();
	return 0;
}