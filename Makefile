run: DebugGBemu
	./DebugGBemu TEMP.gb
run-test: DebugGBemu
	./DebugGBemu cpu_instrs.gb
run-default: DebugGBemu
	./DebugGBemu
DebugGBemu: debug_gbemu.cpp
	g++ debug_gbemu.cpp `sdl2-config --libs --cflags` -o DebugGBemu