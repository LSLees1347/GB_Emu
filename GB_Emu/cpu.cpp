#include "cpu.h"
#include "memory.h"
#include <iostream>
#include <cstdlib>

Registers regs;

void initCPU() {
    regs.PC = 0x0100;
    regs.SP = 0xFFFE;
}

void emulateCycle() {
    uint8_t opcode = memory[regs.PC++];

    switch (opcode) {
    case 0x00: // NOP
        break;

    case 0x3E: // LD A, immediate 8-bit
        regs.A = memory[regs.PC++];
        break;

    case 0x06: // LD B, immediate 8-bit
        regs.B = memory[regs.PC++];
        break;

    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
        std::exit(1);
    }
}
