#ifndef _GB_EMU_CPU_H_
#define  _GB_EMU_CPU_H_

#include <cstdint>
#include <iostream>

struct Register{
    uint8_t upperByte;
    uint8_t lowerByte;
    Register();
    Register(uint8_t upperByte, uint8_t lowerByte);
    Register(uint16_t val);
    Register(Register const& other);
    operator uint16_t() const;
};

class CPU{
public:
    CPU(); // Constructor w/ initial register state?
private:
    // MMU memory;
    Register AF, BC, DE, HL;
    uint8_t &A = AF.lowerByte;
    uint8_t &F = AF.upperByte;
    uint8_t &B = BC.lowerByte;
    uint8_t &C = BC.upperByte;
    uint8_t &D = DE.lowerByte;
    uint8_t &E = DE.upperByte;
    uint8_t &H = HL.lowerByte;
    uint8_t &L = HL.upperByte;

    uint16_t SP; // consider making Register type
    uint16_t PC;

    uint16_t executeNextOpcode();

    // Instruction set - return type is #(cycles to execute opcode)
    uint16_t NOP(uint16_t address); // NOP (0x00)
};

#endif