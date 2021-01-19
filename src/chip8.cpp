#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <random>
#include "time.h"

#include "chip8.h"

unsigned char chip8Fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


Chip8::Chip8() {}
Chip8::~Chip8() {}

void Chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // Clear memory
    for (int i=0; i <4096; i++){
        memory[i] = 0;
    }
    // Clear graphics
    for (int i=0; i < 2048; i++) {
        graphics[i] = 0;
    }
    // Clear stack, keypad and registers 
    for (int i=0; i < 16; i++) {
        stack[i] = 0;
        keypad[i] = 0;
        V[i] = 0;
    }
    // Load fontset
    for (int i=0; i<80; i++) {
        memory[i] = chip8Fontset[i];
    }

    delayTimer = 0;
    soundTimer = 0;

    srand(time(NULL));
}

bool Chip8::loadROM(const char *file_path) {
    initialize();

    printf("Loading ROM: %s\n", file_path);
    FILE* rom = fopen(file_path, "rb");
    if (rom == NULL) {
        printf("Error opening file unexist.ent: %s\n",strerror(errno));
        return false;
    }

    fseek(rom, 0, SEEK_END);
    uint8_t romSize = ftell(rom);
    rewind(rom);

    char* romBuffer = (char *) malloc(sizeof(char) * romSize);
    if (romBuffer == NULL) {
        printf("Could not allocate memory");
        return false;

    if(romSize < 4096 - 512) {
        for (int i=0; i < romSize; i++) {
            memory[512 + i] = (uint8_t)romBuffer[i];
        }
    } else {
        printf("ROM too large to load in memory");
        return false;
    }
    }

    fclose(rom);
    free(romBuffer);
    return true;
}

void Chip8::emulateCycle() {

    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode opcode and execute
    switch (opcode & 0xF000)
    {
        case 0x000:
            switch (opcode & 0x000F){
                case 0x0000:
                    for (int i=0; i < 2048; i++){
                        graphics[i] = 0;
                    }
                    drawFlag = true;
                    pc += 2;
                    break;
                
                case 0x000E:
                    sp--;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    printf("Unrecognized opcode: %n\n", opcode);
                    exit(3);
            }
            break;

        case 0x1000:
            pc = opcode & 0x0FFF;
            break;
        
        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8]  == (opcode & 0x00FF)) {
                pc += 4;
            } else{
                pc += 2;
            }
            break;

        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8]  != (opcode & 0x00FF)) {
                pc += 4;
            } else{
                pc += 2;
            }
            break;

        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc +=4;
            } else {
                pc +=2;
            }
            break;

        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;

        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc +=2;
            break;

        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0004:
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    if (V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4] > 255) {
                        V[16] = 1;
                    } else{
                        V[16] = 0;
                    }
                    pc += 2;
                    break;

                case 0x0005:
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
                        V[16] = 1;
                    } else{
                        V[16] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                case 0x0006:
                    V[16] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;

                case 0x0007:
                    if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]) {
                        V[16] = 1;
                    } else{
                        V[16] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x000E:
                    V[16] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;

                default:
                    printf("Unrecognized opcode: %n\n", opcode);
                    exit(3);
            }
            break;

        case 0x9000:
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        
        case 0xB000:
            pc = opcode & 0x0FFF + V[0];
            break;
        
        case 0xC000:
            V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            pc +=2;
            break;
        
        case 0xD000:
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(graphics[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            V[0xF] = 1;
                        }
                        graphics[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        }
            break;
        
        case 0xE000:
            switch(opcode & 0x00F0) {
                case 0x0090:
                    if(keypad[V[(opcode & 0x0F00) >> 8]] != 0) {
                        pc +=4;
                    } else {
                        pc +=2;
                    }
                    break;

                case 0x00A0:
                    if(keypad[V[(opcode & 0x0F00) >> 8]] == 0) {
                        pc +=4;
                    } else {
                        pc +=2;
                    }
                    break;
            }
            break;
        
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delayTimer;
                    pc += 2;
                    break;
                
                case 0x000A:
                {
                    bool keyPressed = false;

                    for (int i=0; i < 16; i++) {
                        if (keypad[i] != 0) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            keyPressed = true;

                        }
                    }

                    if(!keyPressed) {
                        return;
                    }
                    
                    pc += 2;
                }
                    break;

                case 0x0015:
                    delayTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0018:
                    soundTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x001E:
                    if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                case 0x0029:
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                case 0x0033:
                    memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
                    pc += 2;
                    break;
                
                case 0x0055:
                    for (int i=0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        memory[I + i] = V[i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc +=2;
                    break;
                
                case 0x0065:
                    for (int i=0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        V[i] = memory[I + i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc +=2;
                    break;

                default:
                    printf("Unrecognized opcode: %n\n", opcode);
                    exit(3);
            }
            break;
        
        default:
            printf("Unrecognized opcode: %n\n", opcode);
            exit(3);
    }

    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0)
        if(soundTimer == 1) {
            // TODO
        }
        --soundTimer;
}