#include "memory.h"

uint8_t memory[0x10000];

void initMemory() {
    for (int i = 0; i < 0x10000; ++i)
        memory[i] = 0;
}

void loadTestProgram() {
    memory[0x0100] = 0x3E; // LD A, 0x42
    memory[0x0101] = 0x42;

    memory[0x0102] = 0x06; // LD B, 0x99
    memory[0x0103] = 0x99;

    memory[0x0104] = 0x00; // NOP
}
