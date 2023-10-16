#ifndef _GB_EMU_CPU_H_
#define  _GB_EMU_CPU_H_

#include "..\inc\memory_map.h"

#include <cstdint>
#include <iostream>
#include <iomanip>

struct HalfRegister final{
    uint8_t byte;
    HalfRegister();
    HalfRegister(uint8_t byte);
    operator uint8_t() const;
    HalfRegister& operator ^=(uint8_t rhs);
    HalfRegister& operator |=(uint8_t rhs);
    HalfRegister& operator &=(uint8_t rhs);
};
struct Register final{
    HalfRegister lowerByte;
    HalfRegister upperByte; // GB is little endian, so should really be other way around, but this is only relevant for testSystemEndianness...
    Register();
    Register(uint8_t lowerByte, uint8_t upperByte);
    Register(uint16_t val);
    operator uint16_t() const;
    Register& operator--();
    Register operator--(int);
    Register& operator++();
    Register operator++(int);
};
class CPU final{
public:
    CPU(); // Constructor w/ initial register state?
    bool start();
private:
    MemoryMap memoryMap;
    Register AF, BC, DE, HL;
    HalfRegister &A = AF.upperByte;
    HalfRegister &F = AF.lowerByte;
    HalfRegister &B = BC.upperByte;
    HalfRegister &C = BC.lowerByte;
    HalfRegister &D = DE.upperByte;
    HalfRegister &E = DE.lowerByte;
    HalfRegister &H = HL.upperByte;
    HalfRegister &L = HL.lowerByte;

    Register SP;
    uint16_t PC;

    uint8_t clockT, clockM; // technically these are each 16bit, but with only upperByte exposed

    uint16_t executeNextOpcode();
    uint16_t executeOpcode(uint8_t opcode);
    uint16_t executeCBOpcode(uint8_t opcode);

    // Instruction set - return type is #(cycles to execute opcode)
    uint16_t NOP(); // NOP (0x00)

    uint16_t readWordAtPC();
    uint8_t readByteAtPC();
    uint16_t LDrrnn(Register& targetReg);
    uint16_t LDnnr(uint16_t targetAddress, HalfRegister dataReg);
    uint16_t XORAr(HalfRegister reg); //  maybe a wrapper for byte would be useful for type safety


    // CB
    uint16_t BITbr(uint8_t bit /*really u3 would be enough!*/, HalfRegister reg);
    uint16_t BITbnn(uint8_t bit, uint16_t address);


    void printOpcode(uint8_t opcode);
    void printOpcodeInfo(uint8_t opcode);

    bool halted = false;
    bool verbose = true;
    std::vector<std::string> opcodeInfo;
    std::vector<std::string> opcodeCBInfo;
};

#endif