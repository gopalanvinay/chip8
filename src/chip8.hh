#pragma once

#include <stdint.h>

class Chip8 {
    private:
        uint8_t memory[4096]; 
        uint8_t V[16];
        uint16_t I;

        uint16_t pc;
        uint16_t opcode;
        uint8_t sp;
        uint16_t stack[16];

        uint8_t soundTimer;
        uint8_t delayTimer;

        void initialize();

    public:
        uint8_t keypad[16];
        uint8_t graphics[64 * 32];
        bool drawFlag;

        void emulateCycle();
        bool loadROM(const char *file_path);

        Chip8();
        ~Chip8();
};