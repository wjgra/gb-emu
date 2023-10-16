#include "..\inc\cpu.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

uint8_t constexpr static FLAG_ZERO = 0x80;
uint8_t constexpr static FLAG_SUBTRACT = 0x40;
uint8_t constexpr static FLAG_HALFCARRY = 0x20;
uint8_t constexpr static FLAG_CARRY = 0x19;

CPU::CPU() : AF{}, BC{}, DE{}, HL{}, SP{}, PC{}{
    // Init opcodeInfo for display in verbose mode
    opcodeInfo = std::vector<std::string>(0x100, "");
    opcodeInfo[0x00] = "NOP";
    opcodeInfo[0x01] = "LD BC from (PC)";
    opcodeInfo[0x11] = "LD DE from (PC)";
    opcodeInfo[0x21] = "LD HL from (PC)";
    opcodeInfo[0x31] = "LD SP from (PC)";
    opcodeInfo[0xA8] = "XOR A with B";
    opcodeInfo[0xA9] = "XOR A with C";
    opcodeInfo[0xAA] = "XOR A with D";
    opcodeInfo[0xAB] = "XOR A with E";
    opcodeInfo[0xAC] = "XOR A with H";
    opcodeInfo[0xAD] = "XOR A with L";
    opcodeInfo[0xAE] = "XOR A with (HL)";
    opcodeInfo[0xAF] = "XOR A with A";


    opcodeInfo[0xCB] = "CB-prefixed opcode! See next line";
    opcodeCBInfo = std::vector<std::string>(0x100, "");
}

bool CPU::start(){
    if (!memoryMap.loadBootProgram(".//input//dmg_boot.bin")){
        return EXIT_FAILURE;
    }
    if (verbose){
        std::cout << "Encountered opcodes:";
    }
    while(executeNextOpcode()){}
    return EXIT_SUCCESS;
}

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
    if (verbose){
        std::cout << "\n";
        printOpcode(opcode);
        std::cout << ": ";
        printOpcodeInfo(opcode);
    }
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
        case 0x32:
            return LDnnr(HL--, A);
            break;
        case 0xA8:
            return XORAr(B);
            break;
        case 0xA9:
            return XORAr(C);
            break;
        case 0xAA:
            return XORAr(D);
            break;
        case 0xAB:
            return XORAr(E);
            break;
        case 0xAC:
            return XORAr(H);
            break;
        case 0xAD:
            return XORAr(L);
            break;
        /* case 0xAE:
            return 0;//XORAnn(HL);
            break; */
        case 0xAF:
            return XORAr(A);
            break;
        case 0xCB:
            return executeCBOpcode(memoryMap.readByte(PC++)); 
            break;
        default:
            if (!verbose){
                std::cout << "Encountered unimplemented opcode ";
                printOpcode(opcode);
                std::cout << "\n";
            }
            else{
                std::cout << "unimplemented!\n";
            }
            return 0;// throw; // Exceptions TBC
        break;
    }    
}

uint16_t CPU::executeCBOpcode(uint8_t opcode){
    if (verbose){
        std::cout << "\n";
        printOpcode(opcode);
        std::cout << ": ";
        printOpcodeInfo(opcode);
    }
    switch(opcode){
        case 0x7c: //  to do : similar opcodes!
            return BITbr(7, H);
            break;
        default:
            if (!verbose){
                std::cout << "Encountered unimplemented opcode ";
                printOpcode(opcode);
                std::cout << "\n";
            }
            else{
                std::cout << "unimplemented!\n";
            }
            return 0;// throw; // Exceptions TBC
        break;
    }  

}

// NOP (0x00)
uint16_t CPU::NOP(){
    return 4; // add const??
}

// LDrrnn (0xN1, N = 0, 1, 2, 3)
// Loads word at (PC) to given register
uint16_t CPU::LDrrnn(Register& targetReg){
    targetReg = readWordAtPC();
    return 12;
}

// XORAr (0xA8 - 0xAD, 0xAF)
// XORs A with given half register and stores result in A
uint16_t CPU::XORAr(HalfRegister reg){
    A ^= reg;
    // set flags??
    if (A == 0){
        F |= FLAG_ZERO;
    }
    else{
        F &= !FLAG_ZERO;
    }
    F &= !FLAG_SUBTRACT;
    F &= !FLAG_HALFCARRY;
    F &= !FLAG_CARRY;
    return 4;
}
// issue: make set/unset flag fn?

// LDnnr (0x32)
// Loads byte in dataReg to (targetAddress)
uint16_t CPU::LDnnr(uint16_t targetAddress, HalfRegister dataReg){
    memoryMap.writeByte(targetAddress, dataReg);
    return 8;
}


uint16_t CPU::BITbr(uint8_t bit, HalfRegister reg){
    if ((0x1 << bit) & reg){
        F &= !FLAG_ZERO;
    }
    else{
        F |= FLAG_ZERO;
    }
    F |= FLAG_HALFCARRY;
    F &= !FLAG_SUBTRACT;
    return 8;
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

void CPU::printOpcodeInfo(uint8_t opcode){
    std::cout << opcodeInfo[opcode];
}