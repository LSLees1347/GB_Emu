#include "cpu.h"
#include "memory.h"
#include <iostream>
#include <cstdlib>


Registers regs;

void initCPU() {
    regs.PC = 0x0100;
    regs.SP = 0xFFFE;
}


void setFlagZero(bool c) { regs.F = (regs.F & ~0x80) | (c ? 0x80 : 0x00); }
void setFlagSub(bool c) { regs.F = (regs.F & ~0x40) | (c ? 0x40 : 0x00); }
void setFlagHalfCarry(bool c) { regs.F = (regs.F & ~0x20) | (c ? 0x20 : 0x00); }


void emulateCycle() {
    uint8_t opcode = memory[regs.PC++];

    switch (opcode) {
    case 0x00: // nop
        break;

    case 0x04: // inc b
        uint8_t x = regs.B;
        regs.B++;
        setFlagZero(regs.B == 0);
        setFlagSub(false);
        setFlagHalfCarry(((x & 0x0F) + 1) > 0x0F);
        break;

    case 0x05: //dec b
    {
        uint8_t x = regs.B;
        regs.B--;
        setFlagZero(regs.B == 0);
        setFlagSub(true);
        setFlagHalfCarry((x & 0x0F) == 0);
        break;

    case 0x14: // inc d
        regs.D++;
        break;

    case 0x15: // dec d
        regs.D--;
        break;

    case 0x24: // inc h
        regs.H++;
        break;

    case 0x25: // dec h
        regs.H--;
        break;


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



    case 0x3E: // ld a, immediate byte
        regs.A = memory[regs.PC++];
        break;

    case 0x06: // ld b, immediate byte
        regs.B = memory[regs.PC++];
        break;



    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
        std::exit(1);
    }
}
