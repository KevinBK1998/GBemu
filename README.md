# GBemu
An emulator for gameboy games created for purposes of learning

## Aim
Play Pokemon Red on GBemu

## Author
Kevin Bijoy Kurian

## Build
g++ test.cpp `sdl2-config --libs --cflags` -o test
g++ debug/debug_tiles.cpp `sdl2-config --libs --cflags` -o debug_tiles

## Additional Info
* ttt.gb - no SGB, no MBC, 32kROM, Japan, None , Works but no sound
* tet.gb - no SGB, no MBC, 32kROM, Japan, Nintendo(old) , Not working 
* red.gb - SGB enabled, MBC3+RAM+BATTERY, 1024kROM, 32kERAM, Non-Japan, Nintendo(new) , Not working 
## Acknowledgments
* Imran Nazar
