#pragma once
#include <cstdint>
#include <string>


extern uint8_t memory[0x10000];

void initMemory();
void loadTestProgram();
bool loadROM(const std::string& filename);
uint8_t read8(uint16_t addr);
void write8(uint16_t addr, uint8_t value);
void postBootSetup();