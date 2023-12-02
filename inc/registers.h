#ifndef _GB_EMU_REGISTERS_H_
#define  _GB_EMU_REGISTERS_H_

#include <iostream>
#include <cstdint>

struct HalfRegister final{
    uint8_t byte;
    HalfRegister();
    HalfRegister(uint8_t byte);
    operator uint8_t() const;
    HalfRegister& operator^=(uint8_t rhs);
    HalfRegister& operator|=(uint8_t rhs);
    HalfRegister& operator&=(uint8_t rhs);
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
    HalfRegister upperByte;
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

#endif