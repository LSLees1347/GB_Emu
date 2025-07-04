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
void setFlagCarry(bool c) { regs.F = (regs.F & ~0x10) | (c ? 0x10 : 0x00); }


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


void pushStack(uint8_t hi, uint8_t lo)
{
    regs.SP -= 1;
    memory[regs.SP] = hi;
    regs.SP -= 1;
    memory[regs.SP] = lo;
    return;
}

void popStack(uint8_t &hi, uint8_t &lo)
{
    hi = memory[regs.SP++];
    lo = memory[regs.SP++];
}


uint8_t add8(uint8_t a, uint8_t b)
{
    uint16_t sum = a + b;
    regs.F = 0x00;

    setFlagZero((sum & 0xFF) == 0);
    setFlagSub(false);
    setFlagHalfCarry(((a & 0xF) + (b & 0xF)) > 0xF);
    setFlagCarry(sum > 0xFF);

    return static_cast<uint8_t>(sum);
}

uint8_t sub8(uint8_t a, uint8_t b)
{
    uint8_t sum = a - b;
    regs.F = 0x00;

    setFlagZero(sum == 0);
    setFlagSub(true);
    setFlagHalfCarry((a & 0xF) < (b & 0xF));
    setFlagCarry(a < b);

    return sum;
}

uint8_t adc8(uint8_t a, uint8_t b)
{
    uint8_t carry = getFlagCarry() ? 0 : 1;
    uint8_t sum = a + b + carry;
    regs.F = 0x00;

    setFlagZero((sum & 0xFF) == 0);
    setFlagSub(false);
    setFlagHalfCarry(((a & 0xF) + (b & 0xF) + carry) > 0xF);
    setFlagCarry(sum > 0xFF);

    return sum;
}

uint8_t and8(uint8_t a, uint8_t b)
{
    uint8_t result = a & b;
    regs.F = 0;

    setFlagZero(result == 0);
    setFlagSub(false);
    setFlagHalfCarry(true);
    setFlagCarry(false);

    return result;
}

uint8_t or8(uint8_t a, uint8_t b)
{
    uint8_t result = a | b;
    regs.F = 0;

    setFlagZero(result == 0);
    setFlagSub(false);
    setFlagHalfCarry(false);
    setFlagCarry(false);

    return result;
}

uint8_t xor8(uint8_t a, uint8_t b)
{
    uint8_t result = a ^ b;
    regs.F = 0;

    setFlagZero(result == 0);
    setFlagSub(false);
    setFlagHalfCarry(false);
    setFlagCarry(false);

    return result;
}

void cp8(uint8_t a, uint8_t b)
{
    regs.F = 0;

    setFlagZero(a - b == 0);
    setFlagSub(true);
    setFlagHalfCarry((a & 0xF) < (b & 0xF));
    setFlagCarry(a < b);

    return;
}

uint8_t sbc8(uint8_t a, uint8_t b)
{
    uint8_t carry = getFlagCarry();
    uint8_t result = a - b - carry;
    regs.F = 0;

    setFlagZero(result == 0);
    setFlagSub(true);
    setFlagHalfCarry((a & 0xF) < ((b & 0xF) + carry));
    setFlagCarry(a < b + carry);

    return result;
}


void error(uint8_t opcode)
{
    std::cerr << "invalid opcode: 0x" << std::hex << (int)opcode << "\n";
    std::exit(1);
}


void emulateCycle() {
    uint8_t opcode = memory[regs.PC++];

    switch (opcode) {

    case 0x00: break; // nop


            // byte increments / decrements
    case 0x3C: regs.A = inc8(regs.A); break; // inc a
    case 0x3D: regs.A = dec8(regs.A); break; // dec a
    case 0x04: regs.B = inc8(regs.B); break; // inc b
    case 0x05: regs.B = dec8(regs.B); break; // dec b
    case 0x0C: regs.C = inc8(regs.C); break; // inc c
    case 0x0D: regs.C = dec8(regs.C); break; // dec c
    case 0x14: regs.D = inc8(regs.D); break; // inc d
    case 0x15: regs.D = dec8(regs.D); break; // dec d
    case 0x1C: regs.E = inc8(regs.E); break; // inc e
    case 0x1D: regs.E = dec8(regs.E); break; // dec e
    case 0x24: regs.H = inc8(regs.H); break; // inc h
    case 0x25: regs.H = dec8(regs.H); break; // dec h
    case 0x2C: regs.L = inc8(regs.L); break; // inc l
    case 0x2D: regs.L = dec8(regs.L); break; // dec l




            // 16 bit increments
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
    case 0x33: { regs.SP++; break; } // inc sp


             // 16 bit decrements
    case 0x0B: // dec bc
    {
        uint16_t x = regs.BC();
        regs.setBC(--x);
        break;
    }
    case 0x1B: // dec de
    {
        uint16_t x = regs.DE();
        regs.setDE(--x);
        break;
    }
    case 0x2B: // dec hl
    {
        uint16_t x = regs.HL();
        regs.setHL(--x);
        break;
    }
    case 0x3B: { regs.SP--; break; } // dec sp





             // load immediate byte
    case 0x06: regs.B = memory[regs.PC++]; break; // ld b, n
    case 0x0E: regs.C = memory[regs.PC++]; break; // ld c, n
    case 0x16: regs.D = memory[regs.PC++]; break; // ld d, n
    case 0x1E: regs.E = memory[regs.PC++]; break; // ld e, n
    case 0x26: regs.H = memory[regs.PC++]; break; // ld h, n
    case 0x2E: regs.L = memory[regs.PC++]; break; // ld l, n
    case 0x3E: regs.A = memory[regs.PC++]; break; // ld a, n


        // load immediate 2 bytes
    case 0x01: // ld bc, u8
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.setBC((hi << 8) | lo);
        break;
    }
    case 0x11: // ld de, u8
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.setDE((hi << 8) | lo);
        break;
    }
    case 0x21: // ld hl, u8
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.setHL((hi << 8) | lo);
        break;
    }
    case 0x31: // ld sp, u8
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.SP = (hi << 8) | lo;
        break;
    }



    case 0x02: { memory[regs.BC()] = regs.A; break; } // ld (bc), a
    case 0x12: { memory[regs.DE()] = regs.A; break; } // ld (de), a




    case 0xEA: // ld (a16), a
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        memory[(hi << 8) | lo] = regs.A;
        break;
    }
    case 0xFA: // ld a, (a16)
    {
        uint8_t lo = memory[regs.PC++];
        uint8_t hi = memory[regs.PC++];
        regs.A = memory[(hi << 8) | lo];
        break;
    }




    case 0xF2: { regs.A = memory[0xFF00 + regs.C]; break; } // ld a, (c)
    case 0xE2: { memory[0xFF00 + regs.C] = regs.A; break; } // ld (c), a



    case 0x2A: {
        regs.A = memory[regs.HL()];
        regs.setHL(regs.HL() + 1); break;
    } // ld a, (hl+)
    case 0x3A: {
        regs.A = memory[regs.HL()];
        regs.setHL(regs.HL() - 1); break;
    } // ld a, (hl-)
    case 0x22: {
        memory[regs.HL()] = regs.A;
        regs.setHL(regs.HL() + 1); break;
    } // ld (hl+), a
    case 0x32: {
        memory[regs.HL()] = regs.A;
        regs.setHL(regs.HL() - 1); break;
    } // ld (hl-), a





            // ld a, r
    case 0x78: { regs.A = regs.B; break; } // ld a, b
    case 0x79: { regs.A = regs.C; break; } // ld a, c
    case 0x7A: { regs.A = regs.D; break; } // ld a, d
    case 0x7B: { regs.A = regs.E; break; } // ld a, e
    case 0x7C: { regs.A = regs.H; break; } // ld a, h
    case 0x7D: { regs.A = regs.L; break; } // ld a, l
    case 0x7E: { regs.A = memory[regs.HL()]; break; } // ld a, (hl)
    case 0x7F: { regs.A = regs.A; break; } // ld a, a

             // ld b, r
    case 0x40: { regs.B = regs.B; break; } // ld b, b
    case 0x41: { regs.B = regs.C; break; } // ld b, c
    case 0x42: { regs.B = regs.D; break; } // ld b, d
    case 0x43: { regs.B = regs.E; break; } // ld b, e
    case 0x44: { regs.B = regs.H; break; } // ld b, h
    case 0x45: { regs.B = regs.L; break; } // ld b, l
    case 0x46: { regs.B = memory[regs.HL()]; break; } // ld b, (hl)
    case 0x47: { regs.B = regs.A; break; } // ld b, a

             // ld c, r
    case 0x48: { regs.C = regs.B; break; } // ld c, b
    case 0x49: { regs.C = regs.C; break; } // ld c, c
    case 0x4A: { regs.C = regs.D; break; } // ld c, d
    case 0x4B: { regs.C = regs.E; break; } // ld c, e
    case 0x4C: { regs.C = regs.H; break; } // ld c, h
    case 0x4D: { regs.C = regs.L; break; } // ld c, l
    case 0x4E: { regs.C = memory[regs.HL()]; break; } // ld c, (hl)
    case 0x4F: { regs.C = regs.A; break; } // ld c, a

             // ld d, r
    case 0x50: { regs.D = regs.B; break; } // ld d, b
    case 0x51: { regs.D = regs.C; break; } // ld d, c
    case 0x52: { regs.D = regs.D; break; } // ld d, d
    case 0x53: { regs.D = regs.E; break; } // ld d, e
    case 0x54: { regs.D = regs.H; break; } // ld d, h
    case 0x55: { regs.D = regs.L; break; } // ld d, l
    case 0x56: { regs.D = memory[regs.HL()]; break; } // ld d, (hl)
    case 0x57: { regs.D = regs.A; break; } // ld d, a

             // ld e, r
    case 0x58: { regs.E = regs.B; break; } // ld e, b
    case 0x59: { regs.E = regs.C; break; } // ld e, c
    case 0x5A: { regs.E = regs.D; break; } // ld e, d
    case 0x5B: { regs.E = regs.E; break; } // ld e, e
    case 0x5C: { regs.E = regs.H; break; } // ld e, h
    case 0x5D: { regs.E = regs.L; break; } // ld e, l
    case 0x5E: { regs.E = memory[regs.HL()]; break; } // ld e, (hl)
    case 0x5F: { regs.E = regs.A; break; } // ld e, a

             // ld h, r
    case 0x60: { regs.H = regs.B; break; } // ld h, b
    case 0x61: { regs.H = regs.C; break; } // ld h, c
    case 0x62: { regs.H = regs.D; break; } // ld h, d
    case 0x63: { regs.H = regs.E; break; } // ld h, e
    case 0x64: { regs.H = regs.H; break; } // ld h, h
    case 0x65: { regs.H = regs.L; break; } // ld h, l
    case 0x66: { regs.H = memory[regs.HL()]; break; } // ld h, (hl)
    case 0x67: { regs.H = regs.A; break; } // ld h, a

             // ld l, r
    case 0x68: { regs.L = regs.B; break; } // ld l, b
    case 0x69: { regs.L = regs.C; break; } // ld l, c
    case 0x6A: { regs.L = regs.D; break; } // ld l, d
    case 0x6B: { regs.L = regs.E; break; } // ld l, e
    case 0x6C: { regs.L = regs.H; break; } // ld l, h
    case 0x6D: { regs.L = regs.L; break; } // ld l, l
    case 0x6E: { regs.L = memory[regs.HL()]; break; } // ld l, (hl)
    case 0x6F: { regs.L = regs.A; break; } // ld l, a

             // ld (hl), r
    case 0x70: { memory[regs.HL()] = regs.B; break; } // ld (hl), b
    case 0x71: { memory[regs.HL()] = regs.C; break; } // ld (hl), c
    case 0x72: { memory[regs.HL()] = regs.D; break; } // ld (hl), d
    case 0x73: { memory[regs.HL()] = regs.E; break; } // ld (hl), e
    case 0x74: { memory[regs.HL()] = regs.H; break; } // ld (hl), h
    case 0x75: { memory[regs.HL()] = regs.L; break; } // ld (hl), l
    case 0x77: { memory[regs.HL()] = regs.A; break; } // ld (hl), a




             // push to stack
    case 0xC5: { pushStack(regs.B, regs.C); break; } // push bc
    case 0xD5: { pushStack(regs.D, regs.E); break; } // push de
    case 0xE5: { pushStack(regs.H, regs.L); break; } // push hl
    case 0xF5: { pushStack(regs.A, regs.F & 0xF0); break; } // push af



             // pop from stack
    case 0xC1: { popStack(regs.B, regs.C); break; } // pop bc
    case 0xD1: { popStack(regs.D, regs.E); break; } // pop de
    case 0xE1: { popStack(regs.H, regs.L); break; } // pop hl
    case 0xF1: { popStack(regs.A, regs.F); regs.F &= 0xF0b; break; } // pop af





             // add
    case 0x87: { regs.A = add8(regs.A, regs.A); break; } // add a, a
    case 0x80: { regs.A = add8(regs.A, regs.B); break; } // add a, b
    case 0x81: { regs.A = add8(regs.A, regs.C); break; } // add a, c
    case 0x82: { regs.A = add8(regs.A, regs.D); break; } // add a, d
    case 0x83: { regs.A = add8(regs.A, regs.E); break; } // add a, e
    case 0x84: { regs.A = add8(regs.A, regs.H); break; } // add a, h
    case 0x85: { regs.A = add8(regs.A, regs.L); break; } // add a, l
    case 0x86: { regs.A = add8(regs.A, memory[regs.HL()]); break; } // add a, (hl)
    case 0xC6: { regs.A = add8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // add a, u8

             // sub
    case 0x97: { regs.A = sub8(regs.A, regs.A); break; } // sub a, a
    case 0x90: { regs.A = sub8(regs.A, regs.B); break; } // sub a, b
    case 0x91: { regs.A = sub8(regs.A, regs.C); break; } // sub a, c
    case 0x92: { regs.A = sub8(regs.A, regs.D); break; } // sub a, d
    case 0x93: { regs.A = sub8(regs.A, regs.E); break; } // sub a, e
    case 0x94: { regs.A = sub8(regs.A, regs.H); break; } // sub a, h
    case 0x95: { regs.A = sub8(regs.A, regs.L); break; } // sub a, l
    case 0x96: { regs.A = sub8(regs.A, memory[regs.HL()]); break; } // sub a, (hl)
    case 0xD6: { regs.A = sub8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // sub a, u8

             // adc
    case 0x88: { regs.A = adc8(regs.A, regs.B); break; } // adc a, b
    case 0x89: { regs.A = adc8(regs.A, regs.C); break; } // adc a, c
    case 0x8A: { regs.A = adc8(regs.A, regs.D); break; } // adc a, d
    case 0x8B: { regs.A = adc8(regs.A, regs.E); break; } // adc a, e
    case 0x8C: { regs.A = adc8(regs.A, regs.H); break; } // adc a, h
    case 0x8D: { regs.A = adc8(regs.A, regs.L); break; } // adc a, l
    case 0x8F: { regs.A = adc8(regs.A, regs.A); break; } // adc a, a
    case 0x8E: { regs.A = adc8(regs.A, memory[regs.HL()]); break; } // adc a, (hl)
    case 0xCE: { regs.A = adc8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // adc a, u8

             // and
    case 0xA7: { regs.A = and8(regs.A, regs.A); break; } // and a, b
    case 0xA0: { regs.A = and8(regs.A, regs.B); break; } // and a, b
    case 0xA1: { regs.A = and8(regs.A, regs.C); break; } // and a, c
    case 0xA2: { regs.A = and8(regs.A, regs.D); break; } // and a, d
    case 0xA3: { regs.A = and8(regs.A, regs.E); break; } // and a, e
    case 0xA4: { regs.A = and8(regs.A, regs.H); break; } // and a, h
    case 0xA5: { regs.A = and8(regs.A, regs.L); break; } // and a, l
    case 0xA6: { regs.A = and8(regs.A, memory[regs.HL()]); break; } // and a, (hl)
    case 0xE6: { regs.A = and8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // and a, u8

             // or
    case 0xB7: { regs.A = or8(regs.A, regs.A); break; } // or a, a
    case 0xB0: { regs.A = or8(regs.A, regs.B); break; } // or a, b
    case 0xB1: { regs.A = or8(regs.A, regs.C); break; } // or a, c
    case 0xB2: { regs.A = or8(regs.A, regs.D); break; } // or a, d
    case 0xB3: { regs.A = or8(regs.A, regs.E); break; } // or a, e
    case 0xB4: { regs.A = or8(regs.A, regs.H); break; } // or a, h
    case 0xB5: { regs.A = or8(regs.A, regs.L); break; } // or a, l
    case 0xB6: { regs.A = or8(regs.A, memory[regs.HL()]); break; } // or a, (hl)
    case 0xF6: { regs.A = or8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // or a, u8

             // xor
    case 0xAF: { regs.A = xor8(regs.A, regs.A); break; } // xor a, a
    case 0xA8: { regs.A = xor8(regs.A, regs.B); break; } // xor a, b
    case 0xA9: { regs.A = xor8(regs.A, regs.C); break; } // xor a, c
    case 0xAA: { regs.A = xor8(regs.A, regs.D); break; } // xor a, d
    case 0xAB: { regs.A = xor8(regs.A, regs.E); break; } // xor a, e
    case 0xAC: { regs.A = xor8(regs.A, regs.H); break; } // xor a, h
    case 0xAD: { regs.A = xor8(regs.A, regs.L); break; } // xor a, l
    case 0xAE: { regs.A = xor8(regs.A, memory[regs.HL()]); break; } // xor a, (hl)
    case 0xEE: { regs.A = xor8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // xor a, u8

             // cp
    case 0xBF: { cp8(regs.A, regs.A); break; } // cp a, a
    case 0xB8: { cp8(regs.A, regs.B); break; } // cp a, b
    case 0xB9: { cp8(regs.A, regs.C); break; } // cp a, c
    case 0xBA: { cp8(regs.A, regs.D); break; } // cp a, d
    case 0xBB: { cp8(regs.A, regs.E); break; } // cp a, e
    case 0xBC: { cp8(regs.A, regs.H); break; } // cp a, h
    case 0xBD: { cp8(regs.A, regs.L); break; } // cp a, l
    case 0xBE: { cp8(regs.A, memory[regs.HL()]); break; } // cp a, (hl)
    case 0xFE: { cp8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // cp a, u8

             // sbc
    case 0x9F: { regs.A = sbc8(regs.A, regs.A); break; } // sbc a, a
    case 0x98: { regs.A = sbc8(regs.A, regs.B); break; } // sbc a, b
    case 0x99: { regs.A = sbc8(regs.A, regs.C); break; } // sbc a, c
    case 0x9A: { regs.A = sbc8(regs.A, regs.D); break; } // sbc a, d
    case 0x9B: { regs.A = sbc8(regs.A, regs.E); break; } // sbc a, e
    case 0x9C: { regs.A = sbc8(regs.A, regs.H); break; } // sbc a, h
    case 0x9D: { regs.A = sbc8(regs.A, regs.L); break; } // sbc a, l
    case 0x9E: { regs.A = sbc8(regs.A, memory[regs.HL()]); break; } // sbc a, (hl)
    case 0xDE: { regs.A = sbc8(regs.A, memory[regs.PC + 1]); regs.PC++; break; } // sbc a, u8





    default: { error(opcode); break; }
           /*
           null inrange opcodes:
           0xD3 0xE3
           0xE4 0xF4
           0xD3 0xDB
           0xEB 0xEC
           0xFC 0xDD
           0xED 0xFD
           */

    }
}