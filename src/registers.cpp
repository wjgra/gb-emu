#include "..\inc\registers.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

HalfRegister::HalfRegister() : byte{}{
}

HalfRegister::HalfRegister(uint8_t byte) : byte{byte}{
}

HalfRegister::operator uint8_t() const{
    return byte;
}

HalfRegister& HalfRegister::operator^=(uint8_t rhs){
    byte ^= rhs;
    return *this;
}

HalfRegister& HalfRegister::operator|=(uint8_t rhs){
    byte |= rhs;
    return *this;
}

HalfRegister& HalfRegister::operator&=(uint8_t rhs){
    byte &= rhs;
    return *this;
}

HalfRegister& HalfRegister::operator+=(uint8_t rhs){
    byte += rhs;
    return *this;
}

HalfRegister& HalfRegister::operator-=(uint8_t rhs){
    byte -= rhs;
    return *this;
}

HalfRegister& HalfRegister::operator--(){
    *this = HalfRegister((uint8_t(*this) - 1));
    return *this;
}

HalfRegister HalfRegister::operator--(int){
    HalfRegister temp {*this};
    operator--();
    return temp;
}

HalfRegister& HalfRegister::operator++(){
    *this = HalfRegister((uint8_t(*this) + 1));
    return *this;
}

HalfRegister HalfRegister::operator++(int){
    HalfRegister temp {*this};
    operator++();
    return temp;
}

// Test nth bit in register
bool HalfRegister::testBit(uint8_t bit) const{
    return ((0b1 << bit) & byte);
}

// Set nth bit in register
void HalfRegister::setBit(uint8_t bit){
    byte |= (0b1 << bit);
}

void HalfRegister::setBit(uint8_t bit, bool value){
    value ? setBit(bit) : clearBit(bit);
}

// Clear nth bit in register
void HalfRegister::clearBit(uint8_t bit){
    byte &= ~(0b1 << bit);
}

Register::Register() : lowerByte{}, upperByte{}{
}

Register::Register(uint8_t lowerByte, uint8_t upperByte) : lowerByte{lowerByte}, upperByte{upperByte}{
}

Register::Register(uint16_t val) : lowerByte(val & LOWER_BYTEMASK), upperByte(val >> 8){
}

Register::operator uint16_t() const{
    return (upperByte << 8) + lowerByte;
}

Register& Register::operator--(){
    *this = Register((uint16_t(*this) - 1));
    return *this;
}

Register Register::operator--(int){
    Register temp {*this};
    operator--();
    return temp;
}

Register& Register::operator++(){
    *this = Register((uint16_t(*this) + 1));
    return *this;
}

Register Register::operator++(int){
    Register temp {*this};
    operator++();
    return temp;
}

Register& Register::operator+=(uint16_t rhs){
    Register temp = uint16_t(*this) + rhs;
    *this = temp;
    return *this;
}

Register& Register::operator-=(uint16_t rhs){
    Register temp = uint16_t(*this) - rhs;
    *this = temp;
    return *this;
}