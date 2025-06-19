#include <iostream>
#include "cpu.h"
#include "memory.h"

int main() {
    initMemory();
    loadTestProgram();

    initCPU();

    for (int i = 0; i < 3; ++i) {
        emulateCycle();
    }

    std::cout << "A = 0x" << std::hex << (int)regs.A << "\n";
    std::cout << "B = 0x" << std::hex << (int)regs.B << "\n";

    return 0;
}