#include "memory.h"
#include <fstream>
#include <vector>
#include <iostream>


uint8_t memory[0x10000]; // 64KB


void initMemory()
{
    for (int i = 0; i < 0x10000; ++i)
        memory[i] = 0;
}

void postBootSetup()
{
    write8(0xFF05, 0x00); // TIMA
    write8(0xFF06, 0x00); // TMA
    write8(0xFF07, 0x00); // TAC
    write8(0xFF10, 0x80); // NR10
    write8(0xFF11, 0xBF); // NR11
    write8(0xFF12, 0xF3); // NR12
    write8(0xFF14, 0xBF); // NR14
    write8(0xFF16, 0x3F); // NR21
    write8(0xFF17, 0x00); // NR22
    write8(0xFF19, 0xBF); // NR24
    write8(0xFF1A, 0x7F); // NR30
    write8(0xFF1B, 0xFF); // NR31
    write8(0xFF1C, 0x9F); // NR32
    write8(0xFF1E, 0xBF); // NR34
    write8(0xFF20, 0xFF); // NR41
    write8(0xFF21, 0x00); // NR42
    write8(0xFF22, 0x00); // NR43
    write8(0xFF23, 0xBF); // NR44
    write8(0xFF24, 0x77); // NR50
    write8(0xFF25, 0xF3); // NR51
    write8(0xFF26, 0xF1); // NR52
    write8(0xFF40, 0x91); // LCDC
    write8(0xFF42, 0x00); // SCY
    write8(0xFF43, 0x00); // SCX
    write8(0xFF45, 0x00); // LYC
    write8(0xFF47, 0xFC); // BGP
    write8(0xFF48, 0xFF); // OBP0
    write8(0xFF49, 0xFF); // OBP1
    write8(0xFF4A, 0x00); // WY
    write8(0xFF4B, 0x00); // WX
    write8(0xFFFF, 0x00); // IE
}

void loadTestProgram()
{
    memory[0x0100] = 0x3E; // LD A, 0x42
    memory[0x0101] = 0x42;

    memory[0x0102] = 0x06; // LD B, 0x99
    memory[0x0103] = 0x99;

    memory[0x0104] = 0x00; // NOP
}

bool loadROM(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "rom.gb couldnt be opened!" << "\n";
        return false;
    }

    file.read(reinterpret_cast<char*>(memory), 0x8000);
    return true;
}

uint8_t read8(uint16_t addr)
{
    return memory[addr];
}

void write8(uint16_t addr, uint8_t value)
{
    memory[addr] = value;
}