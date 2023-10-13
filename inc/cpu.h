#ifndef _GB_EMU_CPU_H_
#define  _GB_EMU_CPU_H_

#include "..\inc\memory_map.h"

#include <cstdint>
#include <iostream>
#include <iomanip>

struct Register final{
    uint8_t lowerByte;
    uint8_t upperByte; // GB is little endian, so should really be other way around, but this is only relevant for testSystemEndianness...
    Register();
    Register(uint8_t lowerByte, uint8_t upperByte);
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
    uint8_t &A = AF.upperByte;
    uint8_t &F = AF.lowerByte;
    uint8_t &B = BC.upperByte;
    uint8_t &C = BC.lowerByte;
    uint8_t &D = DE.upperByte;
    uint8_t &E = DE.lowerByte;
    uint8_t &H = HL.upperByte;
    uint8_t &L = HL.lowerByte;

    Register SP; // consider making Register type
    uint16_t PC;

    uint8_t clockT, clockM; // technically these are each 16bit, but with only upperByte exposed

    uint16_t executeNextOpcode();
    uint16_t executeOpcode(uint8_t opcode);

    // Instruction set - return type is #(cycles to execute opcode)
    uint16_t NOP(); // NOP (0x00)

    template <Register CPU::*reg> uint16_t loadRegisterFromAddress(uint16_t address);
    void writeWordToRegister();
    void readWordFromRegister();

    uint16_t readWordAtPC();
    uint8_t readByteAtPC();
    uint16_t LDrrnn(Register& target); // load word at PC to given register

    uint16_t XORr(uint8_t& reg); //  maybe a wrapper for byte would be useful for type safety

    void printOpcode(uint8_t opcode);
    

    bool halted = false;
};

#endif