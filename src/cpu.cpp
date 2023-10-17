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

    opcodeInfo[0x0E] = "LD C from (PC)";

    opcodeInfo[0x11] = "LD DE from (PC)";

    opcodeInfo[0x18] = "JR by int8_t offset";
    
    opcodeInfo[0x1E] = "LD E from (PC)";

    opcodeInfo[0x20] = "JR if NZ by int8_t offset";
    opcodeInfo[0x21] = "LD HL from (PC)";
    opcodeInfo[0x22] = "LD (HL++) from A";

    opcodeInfo[0x2E] = "LD L from (PC)";

    opcodeInfo[0x31] = "LD SP from (PC)";
    opcodeInfo[0x32] = "LD (HL--) from A";

    opcodeInfo[0x3E] = "LD A from (PC)";

    opcodeInfo[0xA8] = "XOR A with B";
    opcodeInfo[0xA9] = "XOR A with C";
    opcodeInfo[0xAA] = "XOR A with D";
    opcodeInfo[0xAB] = "XOR A with E";
    opcodeInfo[0xAC] = "XOR A with H";
    opcodeInfo[0xAD] = "XOR A with L";
    // opcodeInfo[0xAE] = "XOR A with (HL)";
    opcodeInfo[0xAF] = "XOR A with A";


    opcodeInfo[0xCB] = "CB-prefixed opcode! See next line";


    opcodeInfo[0xE2] = "LD (0xFF00+C) with A";


    opcodeInfo[0xF2] = "LD A with (0xFF00+C)";

    opcodeInfo[0xFB] = "Enable interrupts";

    opcodeCBInfo = std::vector<std::string>(0x100, "");
    // 0x00 - 0x3F
    opcodeCBInfo[0x40] = "BIT 0, B";
    opcodeCBInfo[0x41] = "BIT 0, C";
    opcodeCBInfo[0x42] = "BIT 0, D";
    opcodeCBInfo[0x43] = "BIT 0, E";
    opcodeCBInfo[0x44] = "BIT 0, H";
    opcodeCBInfo[0x45] = "BIT 0, L";
    // BIT 0, (HL)
    opcodeCBInfo[0x47] = "BIT 0, A";
    opcodeCBInfo[0x48] = "BIT 1, B";
    opcodeCBInfo[0x49] = "BIT 1, C";
    opcodeCBInfo[0x4A] = "BIT 1, D";
    opcodeCBInfo[0x4B] = "BIT 1, E";
    opcodeCBInfo[0x4C] = "BIT 1, H";
    opcodeCBInfo[0x4D] = "BIT 1, L";
    // BIT 1, (HL)
    opcodeCBInfo[0x4F] = "BIT 1, A";
    opcodeCBInfo[0x50] = "BIT 2, B";
    opcodeCBInfo[0x51] = "BIT 2, C";
    opcodeCBInfo[0x52] = "BIT 2, D";
    opcodeCBInfo[0x53] = "BIT 2, E";
    opcodeCBInfo[0x54] = "BIT 2, H";
    opcodeCBInfo[0x55] = "BIT 2, L";
    // BIT 2, (HL)
    opcodeCBInfo[0x57] = "BIT 2, A";
    opcodeCBInfo[0x58] = "BIT 3, B";
    opcodeCBInfo[0x59] = "BIT 3, C";
    opcodeCBInfo[0x5A] = "BIT 3, D";
    opcodeCBInfo[0x5B] = "BIT 3, E";
    opcodeCBInfo[0x5C] = "BIT 3, H";
    opcodeCBInfo[0x5D] = "BIT 3, L";
    // BIT 3, (HL)
    opcodeCBInfo[0x5F] = "BIT 3, A";
    opcodeCBInfo[0x60] = "BIT 4, B";
    opcodeCBInfo[0x61] = "BIT 4, C";
    opcodeCBInfo[0x62] = "BIT 4, D";
    opcodeCBInfo[0x63] = "BIT 4, E";
    opcodeCBInfo[0x64] = "BIT 4, H";
    opcodeCBInfo[0x65] = "BIT 4, L";
    // BIT 4, (HL)
    opcodeCBInfo[0x67] = "BIT 4, A";
    opcodeCBInfo[0x68] = "BIT 5, B";
    opcodeCBInfo[0x69] = "BIT 5, C";
    opcodeCBInfo[0x6A] = "BIT 5, D";
    opcodeCBInfo[0x6B] = "BIT 5, E";
    opcodeCBInfo[0x6C] = "BIT 5, H";
    opcodeCBInfo[0x6D] = "BIT 5, L";
    // BIT 5, (HL)
    opcodeCBInfo[0x6F] = "BIT 5, A";
    opcodeCBInfo[0x70] = "BIT 6, B";
    opcodeCBInfo[0x71] = "BIT 6, C";
    opcodeCBInfo[0x72] = "BIT 6, D";
    opcodeCBInfo[0x73] = "BIT 6, E";
    opcodeCBInfo[0x74] = "BIT 6, H";
    opcodeCBInfo[0x75] = "BIT 6, L";
    // BIT 6, (HL)
    opcodeCBInfo[0x77] = "BIT 6, A";
    opcodeCBInfo[0x78] = "BIT 7, B";
    opcodeCBInfo[0x79] = "BIT 7, C";
    opcodeCBInfo[0x7A] = "BIT 7, D";
    opcodeCBInfo[0x7B] = "BIT 7, E";
    opcodeCBInfo[0x7C] = "BIT 7, H";
    opcodeCBInfo[0x7D] = "BIT 7, L";
    // BIT 7, (HL)
    opcodeCBInfo[0x7F] = "BIT 7, A";
    // 0x80 - 0xFF
    
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


        case 0x0E:
            return LDru8(C);
            break;


        case 0x11:
            return LDrrnn(DE);
            break;

        case 0x18:
            return JRe();
            break;

        case 0x1E:
            return LDru8(E);
            break;

        case 0x20:
            return JRcce(FLAG_ZERO, false);
            break;
        case 0x21:
            return LDrrnn(HL);
            break;
        case 0x22:
            return LDnnr(HL++, A);
            break;

        case 0x2E:
            return LDru8(L);
            break;


        case 0x31:
            return LDrrnn(SP);
            break;
        case 0x32:
            return LDnnr(HL--, A);
            break;


        case 0x3E:
            return LDru8(A);
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


        case 0xE2:
            return LDnnr(0xFF00 + C, A);
            break;


        case 0xF2:
            return LDrnn(A, 0xFF00 + C);
            break;


        case 0xFB:
            return EI(); 
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
        printOpcodeCBInfo(opcode);
    }
    switch(opcode){
        case 0x40:
            return BITbr(0, B);
            break;
        case 0x41:
            return BITbr(0, C);
            break;
        case 0x42:
            return BITbr(0, D);
            break;
        case 0x43:
            return BITbr(0, E);
            break;
        case 0x44:
            return BITbr(0, H);
            break;
        case 0x45:
            return BITbr(0, L);
            break;
        // 0x46
        case 0x47:
            return BITbr(0, A);
            break;
        case 0x48:
            return BITbr(1, B);
            break;
        case 0x49:
            return BITbr(1, C);
            break;
        case 0x4A:
            return BITbr(1, D);
            break;
        case 0x4B:
            return BITbr(1, E);
            break;
        case 0x4C:
            return BITbr(1, H);
            break;
        case 0x4D:
            return BITbr(1, L);
            break;
        // 0x4E
        case 0x4F:
            return BITbr(1, A);
            break;
        case 0x50:
            return BITbr(2, B);
            break;
        case 0x51:
            return BITbr(2, C);
            break;
        case 0x52:
            return BITbr(2, D);
            break;
        case 0x53:
            return BITbr(2, E);
            break;
        case 0x54:
            return BITbr(2, H);
            break;
        case 0x55:
            return BITbr(2, L);
            break;
        // 0x56
        case 0x57:
            return BITbr(2, A);
            break;
        case 0x58:
            return BITbr(3, B);
            break;
        case 0x59:
            return BITbr(3, C);
            break;
        case 0x5A:
            return BITbr(3, D);
            break;
        case 0x5B:
            return BITbr(3, E);
            break;
        case 0x5C:
            return BITbr(3, H);
            break;
        case 0x5D:
            return BITbr(3, L);
            break;
        // 0x5E
        case 0x5F:
            return BITbr(3, A);
            break;
        case 0x60:
            return BITbr(4, B);
            break;
        case 0x61:
            return BITbr(4, C);
            break;
        case 0x62:
            return BITbr(4, D);
            break;
        case 0x63:
            return BITbr(4, E);
            break;
        case 0x64:
            return BITbr(4, H);
            break;
        case 0x65:
            return BITbr(4, L);
            break;
        // 0x66
        case 0x67:
            return BITbr(4, A);
            break;
        case 0x68:
            return BITbr(5, B);
            break;
        case 0x69:
            return BITbr(5, C);
            break;
        case 0x6A:
            return BITbr(5, D);
            break;
        case 0x6B:
            return BITbr(5, E);
            break;
        case 0x6C:
            return BITbr(5, H);
            break;
        case 0x6D:
            return BITbr(5, L);
            break;
        // 0x6E
        case 0x6F:
            return BITbr(5, A);
            break;
        case 0x70:
            return BITbr(6, B);
            break;
        case 0x71:
            return BITbr(6, C);
            break;
        case 0x72:
            return BITbr(6, D);
            break;
        case 0x73:
            return BITbr(6, E);
            break;
        case 0x74:
            return BITbr(6, H);
            break;
        case 0x75:
            return BITbr(6, L);
            break;
        // 0x76
        case 0x77:
            return BITbr(6, A);
            break;
        case 0x78:
            return BITbr(7, B);
            break;
        case 0x79:
            return BITbr(7, C);
            break;
        case 0x7A:
            return BITbr(7, D);
            break;
        case 0x7B:
            return BITbr(7, E);
            break;
        case 0x7C:
            return BITbr(7, H);
            break;
        case 0x7D:
            return BITbr(7, L);
            break;
        // 0x7E
        case 0x7F:
            return BITbr(7, A);
            break;
        default:
            if (!verbose){
                std::cout << "Encountered unimplemented CB opcode ";
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

// Loads byte in (dataAddress) to targetReg
uint16_t CPU::LDrnn(HalfRegister& targetReg, uint16_t dataAddress){
    targetReg = memoryMap.readByte(dataAddress);
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

uint16_t CPU::BITbnn(uint8_t bit, uint16_t address){
    return BITbr(bit, memoryMap.readByte(address));
}

uint16_t CPU::EI(){
    interruptsEnabled = true;
    return 4;
}

uint16_t CPU::LDru8(HalfRegister& targetReg){
    targetReg = readByteAtPC();
    return 8;
}

/* uint16_t CPU::JPnn(uint16_t address){

    return 16;
}
 */

uint16_t CPU::JRe(){
    PC += static_cast<int8_t>(readByteAtPC());
    return 12;
}

uint16_t CPU::JRcce(uint8_t condition, bool positiveCondition){
    bool res = condition & F;
    if ((res && positiveCondition) || (!res && !positiveCondition)){
        return JRe();
    }
    else{
        ++PC; // Offset is still read
        return 8;
    }
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

void CPU::printOpcodeCBInfo(uint8_t opcode){
    std::cout << opcodeCBInfo[opcode];
}