# Chip8 Emulator
This is a modern emulator for the Chip8 interpreter. CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. There are a number of classic video games ported to CHIP-8, such as Pong, Space Invaders, Tetris, and Pac-Man. There are also applications like a random maze generator and Conway's Game of Life. These programs are reportedly placed in the public domain, and can be easily found on the Internet.

## Build and Run
Compile:
```
mkdir build
cd build
cmake ..
make
```
Run:
```
./chip8 <ROM file>
```

## Resources & Links
While working on emulating the Chip8 language, I found the following links helpful:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.4
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/