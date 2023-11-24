#ifndef _GB_EMU_CPU_H_
#define  _GB_EMU_CPU_H_

#include "..\inc\memory_map.h"

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <deque>
#include <algorithm>

#include <SDL.h>

struct HalfRegister final{
    uint8_t byte;
    HalfRegister();
    HalfRegister(uint8_t byte);
    operator uint8_t() const;
    HalfRegister& operator ^=(uint8_t rhs);
    HalfRegister& operator |=(uint8_t rhs);
    HalfRegister& operator &=(uint8_t rhs);
    HalfRegister& operator+=(uint8_t rhs);
    HalfRegister& operator-=(uint8_t rhs);
    HalfRegister& operator--();
    HalfRegister operator--(int);
    HalfRegister& operator++();
    HalfRegister operator++(int);
    bool testBit(uint8_t bit) const;
    void setBit(uint8_t bit);
    void setBit(uint8_t bit, bool value);
    void clearBit(uint8_t bit);
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
    Register& operator+=(uint16_t rhs);
    Register& operator-=(uint16_t rhs);
};
class CPU final{
public:
    CPU(MemoryMap& memMap);
    uint16_t executeNextOpcode();
    void handleInterrupts();
    void requestInterrupt(uint8_t interrupt);
    void finish();
    void toggleHalt();
private:
    MemoryMap& memoryMap;
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
    Register PC;

    uint8_t clockT, clockM; // technically these are each 16bit, but with only upperByte exposed
    
    uint16_t executeOpcode(uint8_t opcode);
    uint16_t executeCBOpcode(uint8_t opcode);

    uint16_t readWordAtPC();
    uint8_t readByteAtPC();

    // Instruction set - return type is #(cycles to execute opcode)
    // Key for mnemonics
    // ...
    // ...
    // ?? how to indicate address consistently w/o () ??

    // To do: re-order opcode fn defs to match declaration
    //        transform unneeded (nn) opcodes to (HL), and r to A
    //        remove any cout statements?

    uint16_t NOP();
    uint16_t STOP(); // Unimplemented

    // Load operations
    uint16_t LDrru16(Register& targetReg);
    uint16_t LDnnr(uint16_t targetAddress, HalfRegister dataReg);
    uint16_t LDrnn(HalfRegister& targetReg, uint16_t dataAddress);
    uint16_t LDru8(HalfRegister& targetReg);
    uint16_t LDu16rr(Register& dataReg);
    uint16_t LDu16r(HalfRegister& dataReg);
    uint16_t LDrr(HalfRegister& targetReg, HalfRegister& dataReg);
    uint16_t LDFFu8r(HalfRegister& dataReg);
    uint16_t LDrFFu8(HalfRegister& targetReg);
    uint16_t LDrrrr(Register& targetReg, Register& dataReg);
    uint16_t LDru16(HalfRegister& targetReg);
    uint16_t LDHLSPe();

    // Stack operations
    uint16_t POPrr(Register& targetReg);
    uint16_t POPAF();
    uint16_t PUSHrr(Register& dataReg);
    uint16_t PUSHAF();

    // Logical operations
    uint16_t XORAr(HalfRegister reg);
    uint16_t XORAnn(uint16_t dataAddress);
    uint16_t XORAu8();
    uint16_t ORAr(HalfRegister reg);
    uint16_t ORAnn(uint16_t dataAddress);
    uint16_t ORAu8();
    uint16_t ANDAr(HalfRegister reg);
    uint16_t ANDAnn(uint16_t dataAddress);
    uint16_t ANDAu8();

    // Arithmetic operations
    uint16_t INCrr(Register& reg);
    uint16_t INCr(HalfRegister& reg);
    uint16_t INCnn(uint16_t targetAddress);
    uint16_t DECrr(Register& reg);
    uint16_t DECr(HalfRegister& reg);
    uint16_t DECnn(uint16_t targetAddress);
    uint16_t ADDrrrr(Register& x, Register y);
    uint16_t ADDrr(HalfRegister& x, HalfRegister y); // second arg does not need ref type?
    uint16_t ADDrnn(HalfRegister& x, uint16_t targetAddress);
    uint16_t ADDru8(HalfRegister& reg);
    uint16_t ADDSPi8();
    uint16_t SUBrr(HalfRegister& x, HalfRegister& y);
    uint16_t SUBrnn(HalfRegister& x, uint16_t targetAddress);
    uint16_t SUBru8(HalfRegister& reg);

    // Additonal arithmetic operations
    uint16_t ADCAr(HalfRegister reg);
    uint16_t ADCAHL();
    uint16_t ADCAu8();
    uint16_t SBCAr(HalfRegister reg);
    uint16_t SBCAHL();
    uint16_t SBCAu8();

    // Compare registers
    uint16_t CPrr(HalfRegister& x, HalfRegister& y);
    uint16_t CPrnn(HalfRegister& reg, uint16_t targetAddress);
    uint16_t CPru8(HalfRegister& reg);
    uint16_t CPL();

    // Bit operations (test, set, clear)
    uint16_t BITbr(uint8_t bit /*really u3 would be enough!*/, HalfRegister reg);
    uint16_t BITbnn(uint8_t bit, uint16_t address);
    uint16_t SETbr(uint8_t bit, HalfRegister reg);
    uint16_t SETbnn(uint8_t bit, uint16_t address);
    uint16_t RESbr(uint8_t bit, HalfRegister reg);
    uint16_t RESbnn(uint8_t bit, uint16_t address);

    // Enable/disable interrupts
    uint16_t EI();
    uint16_t DI();

    // Rotate - old carry flag rotated in, carry flag set to rotated out value 
    uint16_t RLr(HalfRegister& reg);
    uint16_t RRr(HalfRegister& reg);
    uint16_t RLHL();
    uint16_t RRHL();

    // Rotate circular - a cyclic permutation, carry flag set as before
    uint16_t RLCr(HalfRegister& reg);
    uint16_t RRCr(HalfRegister& reg);
    uint16_t RLCHL();
    uint16_t RRCHL();

    // Special rotations
    uint16_t RLCA();
    uint16_t RLA();
    uint16_t RRCA();
    uint16_t RRA();

    // Shift/swap operations
    uint16_t SLAr(HalfRegister& reg);
    uint16_t SLAHL();
    uint16_t SRAr(HalfRegister& reg);
    uint16_t SRAHL();
    uint16_t SRLr(HalfRegister& reg);
    uint16_t SRLHL();
    uint16_t SWAPr(HalfRegister& reg);
    uint16_t SWAPHL();

    // Jump operations
    uint16_t JPnn(uint16_t address);
    uint16_t JPu16();
    uint16_t JPccu16(uint8_t condition, bool positiveCondition);
    uint16_t JRe();
    uint16_t JRcce(uint8_t condition, bool positiveCondition);

    // Control flow
    uint16_t CALLu16();
    uint16_t CALLccu16(uint8_t condition, bool positiveCondition);
    uint16_t RET();
    uint16_t RETcc(uint8_t condition, bool positiveCondition);
    uint16_t RETI();
    uint16_t RST(uint8_t address);

    // Utility functions
    
    void setFlag(uint8_t flag);
    void setFlag(uint8_t flag, bool value);
    void clearFlag(uint8_t flag);
    bool isFlagSet(uint8_t flag);

    void initOpcodeInfo();

    void printOpcode(uint8_t opcode);
    void printOpcodeInfo(uint8_t opcode);
    void printOpcodeCBInfo(uint8_t opcode);

    bool halted = false;
    bool interruptsEnabled = false;
    std::vector<std::string> opcodeInfo;
    std::vector<std::string> opcodeCBInfo;

    void addOpcodeToLog(uint8_t opcode);
    void printRecentOpcodes();
    unsigned int const maxOpcodeLookback = 8;
    std::deque<uint8_t> recentOpcodes;
};

#endif