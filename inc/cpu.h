#ifndef _GB_EMU_CPU_H_
#define  _GB_EMU_CPU_H_

#include "..\inc\memory_map.h"

#include <cstdint>
#include <iostream>
#include <iomanip>

struct Register final{
    uint8_t upperByte;
    uint8_t lowerByte;
    Register();
    Register(uint8_t upperByte, uint8_t lowerByte);
    Register(uint16_t val);
    Register(Register const& other);
    operator uint16_t() const;
};

class CPU final{
public:
    CPU(); // Constructor w/ initial register state?
    bool start();
private:
    MemoryMap memoryMap;
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

    uint8_t clockT, clockM; // technically these are each 16bit, but with only upperByte exposed

    uint16_t executeNextOpcode();
    uint16_t executeOpcode(uint8_t opcode);

    // Instruction set - return type is #(cycles to execute opcode)
    uint16_t NOP(); // NOP (0x00)

    /* template <Register CPU::*reg> // can use template here, but can only specialise in the .cpp!
    uint16_t loadReg(uint16_t add){
        this->*reg = add;
        return 4;
    } */
    bool halted = false;
};

#endif