#include "..\inc\cpu.h"

CPU::CPU(){
}

Register::Register() : upperByte{}, lowerByte{}{
}

Register::Register(uint8_t upperByte, uint8_t lowerByte) : upperByte{upperByte}, lowerByte{lowerByte}{
}

Register::Register(uint16_t val) : upperByte(val >> 8), lowerByte(val & 0x00FF){
}

Register::Register(Register const& other) : upperByte{other.upperByte}, lowerByte{other.lowerByte}{
}

Register::operator uint16_t() const{
    return (upperByte << 8) + lowerByte;
}

uint16_t CPU::executeNextOpcode(){
    uint16_t numCycles = 0;
    // opcode = read from memory at @PC
    // ++PC;
    // numCycles = executeOpcode(opcode)
    return numCycles;
}

// Flags
enum {
    flag_zero = 0x80,
    flag_subtract = 0x40,
    flag_halfCarry = 0x20,
    flag_carry = 0x19
};

// Instruction set (opcodes)

// NOP (0x00)
uint16_t CPU::NOP(uint16_t){
    return 4;
}


