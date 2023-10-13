#include "..\inc\cpu.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

CPU::CPU() : AF{}, BC{}, DE{}, HL{}, SP{}, PC{}{
}

bool CPU::start(){
    if (!memoryMap.loadBootProgram(".//input//dmg_boot.bin")){
        return EXIT_FAILURE;
    }
    while(executeNextOpcode()){}
    return EXIT_SUCCESS;
}

Register::Register() : lowerByte{}, upperByte{}{
}

Register::Register(uint8_t lowerByte, uint8_t upperByte) : lowerByte{lowerByte}, upperByte{upperByte}{
}

Register::Register(uint16_t val) : lowerByte(val & LOWER_BYTEMASK), upperByte(val >> 8){
}

Register::Register(Register const& other) :  lowerByte{other.lowerByte}, upperByte{other.upperByte}{
}

Register::operator uint16_t() const{
    return (upperByte << 8) + lowerByte;
}

uint16_t CPU::executeNextOpcode(){
    if (halted){
        return NOP();
    }
    else{
        uint8_t opcode = memoryMap.readByte(PC++);
        return executeOpcode(opcode);
    }
}

uint16_t CPU::executeOpcode(uint8_t opcode){
    printOpcode(opcode);
    std::cout << ":\n"; // Temp.
    switch(opcode){
        case 0x00: 
            return NOP();
            break;
        case 0x01:
            return LDrrnn(BC);
            break;
        case 0x11:
            return LDrrnn(DE);
            break;
        case 0x21:
            return LDrrnn(HL);
            break;
        case 0x31:
            return LDrrnn(SP);
            break;
        case 0xAF:
            return XORr(A);
            break;
        default:
            std::cout << "Encountered unimplemented opcode ";
            printOpcode(opcode);
            std::cout << "\n";
            return 0;// throw; // Exceptions TBC
        break;
    }

    
}

// Flags

uint8_t constexpr static flag_zero = 0x80,
    flag_subtract = 0x40,
    flag_halfCarry = 0x20,
    flag_carry = 0x19;

// Instruction set (opcodes)

// NOP (0x00)
uint16_t CPU::NOP(){
    return 4; // add const??
}

template <Register CPU::*reg> uint16_t CPU::loadRegisterFromAddress(uint16_t address){
    this->*reg = memoryMap.readWord(address);
    return 4; //???
}

// loadRegisterFromAddress<&CPU::AF>;

    /* std::cout << "\n" << uint16_t(AF) << ", ";
    loadReg<&CPU::AF>(0x1234);
    std::cout << std::hex << "\n" << uint16_t(AF) << "\n"; */

uint16_t CPU::LDrrnn(Register& target){
    target = readWordAtPC();
    std::cout << std::hex << target << "\n";
    return 12;
}

uint16_t CPU::XORr(uint8_t& reg){
    A ^= reg;
    // set flags??
    if (A == 0){
        // set zero flag
        F |= flag_zero;
    }
    else{
        F |= !flag_zero;
    }
    F |= !flag_subtract;
    F |= !flag_halfCarry;
    F |= !flag_carry;
    return 4;
}

uint8_t CPU::readByteAtPC(){
    return memoryMap.readByte(PC++); 
}

uint16_t CPU::readWordAtPC(){
    auto temp = memoryMap.readWord(PC);
    PC += 2;
    return temp;
}

void CPU::printOpcode(uint8_t opcode){
    std::cout << "0x" << std::setfill('0') << std::setw(2)
                      << std::hex << int(opcode);
}