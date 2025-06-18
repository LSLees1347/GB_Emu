#include <iostream>




struct Registers {
    uint8_t A, F; // accumulator & flags
    uint8_t B, C;
    uint8_t D, E;
    uint8_t H, L;
    uint16_t SP; // stack pointer
    uint16_t PC; // program counter

    uint16_t AF() const { return (A << 8) | F; }
    void setAF(uint16_t val) { A = val >> 8; F = val & 0xF0; }

    uint16_t BC() const { return (B << 8) | C; }
    void setBC(uint16_t val) { B = val >> 8; C = val & 0xFF; }

    uint16_t DE() const { return (D << 8) | E; }
    void setDE(uint16_t val) { D = val >> 8; E = val & 0xFF; }

    uint16_t HL() const { return (H << 8) | L; }
    void setHL(uint16_t val) { H = val >> 8; L = val & 0xFF; }
};


uint8_t memory[0x10000]; // 64kb addressable memory






int main()
{
    std::cout << "Hello World!\n";
    return 0;
}