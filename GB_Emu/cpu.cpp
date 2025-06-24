#include "cpu.h"
#include "memory.h"
#include <iostream>
#include <cstdlib>


Registers regs;

void initCPU() {
    regs.PC = 0x0100;
    regs.SP = 0xFFFE;
}

bool getFlagZero() { return regs.F & 0x80; }
bool getFlagSub() { return regs.F & 0x40; }
bool getFlagHalfCarry() { return regs.F & 0x20; }
bool getFlagCarry() { return regs.F & 0x10; }

void setFlagZero(bool c) { regs.F = (regs.F & ~0x80) | (c ? 0x80 : 0x00); }
void setFlagSub(bool c) { regs.F = (regs.F & ~0x40) | (c ? 0x40 : 0x00); }
void setFlagHalfCarry(bool c) { regs.F = (regs.F & ~0x20) | (c ? 0x20 : 0x00); }


uint8_t inc8(uint8_t x)
{
    setFlagHalfCarry(((x & 0x0F) + 1) > 0x0F);
    x++;
    setFlagZero(x == 0);
    setFlagSub(false);
    return x;
}

uint8_t dec8(uint8_t x)
{
    setFlagHalfCarry((x & 0x0F) == 0);
    x--;
    setFlagZero(x == 0x00);
    setFlagSub(true);
    return x;
}


void emulateCycle() {
    uint8_t opcode = memory[regs.PC++];

    switch (opcode) {

    case 0x00: break; // nop

    case 0x04: regs.B = inc8(regs.B); break; // inc b
    case 0x05: regs.B = dec8(regs.B); break; // dec b
    case 0x0C: regs.C = inc8(regs.C); break; // inc c
    case 0x0D: regs.C = dec8(regs.C); break; // dec c
    case 0x14: regs.D = inc8(regs.D); break; // inc d
    case 0x15: regs.D = dec8(regs.D); break; // dec d
    case 0x24: regs.H = inc8(regs.H); break; // inc h
    case 0x25: regs.H = dec8(regs.H); break; // dec h


    case 0x06: regs.B = memory[regs.PC++]; break; // ld b, n
    case 0x0E: regs.C = memory[regs.PC++]; break; // ld c, n
    case 0x16: regs.D = memory[regs.PC++]; break; // ld d, n
    case 0x1E: regs.E = memory[regs.PC++]; break; // ld e, n
    case 0x26: regs.H = memory[regs.PC++]; break; // ld h, n
    case 0x2E: regs.L = memory[regs.PC++]; break; // ld l, n
    case 0x3E: regs.A = memory[regs.PC++]; break; // ld a, n


    case 0x01: // ld bc, n
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.setBC((hi << 8) | lo);
        break;
    }

    case 0x11: // ld de, n
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.setDE((hi << 8) | lo);
        break;
    }

    case 0x21: // ld hl, n
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.setHL((hi << 8) | lo);
        break;
    }

    case 0x31: // ld sp, n
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.SP = (hi << 8) | lo;
        break;
    }

    case 0x03: // inc bc
    {
        uint16_t x = regs.BC();
        regs.setBC(++x);
        break;
    }

    case 0x13: // inc de
    {
        uint16_t x = regs.DE();
        regs.setDE(++x);
        break;
    }

    case 0x23: // inc hl
    {
        uint16_t x = regs.HL();
        regs.setHL(++x);
        break;
    }

    case 0x33: // inc sp
        regs.SP++;
        break;


    default:
    {
        std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
        std::exit(1);
        break;
    }

    }
}