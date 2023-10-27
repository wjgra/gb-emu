#include "..\inc\cpu.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

uint8_t constexpr static FLAG_ZERO = 0x80;
uint8_t constexpr static FLAG_SUBTRACT = 0x40;
uint8_t constexpr static FLAG_HALFCARRY = 0x20;
uint8_t constexpr static FLAG_CARRY = 0x19;

CPU::CPU() : AF{}, BC{}, DE{}, HL{}, SP{}, PC{}{
    // Opcode info for displaying recent instructions
    opcodeInfo = std::vector<std::string>(0x100, "");
    opcodeInfo[0x00] = "NOP";
    opcodeInfo[0x01] = "LD BC from (PC)";
    opcodeInfo[0x02] = "LD (BC) from A";
    opcodeInfo[0x03] = "INC BC";
    opcodeInfo[0x04] = "INC B";
    opcodeInfo[0x05] = "DEC B";
    opcodeInfo[0x06] = "LD B from (PC)";
    opcodeInfo[0x07] = "RLCA";
    opcodeInfo[0x08] = "LD (PC) from SP";
    opcodeInfo[0x09] = "ADD HL, BC";
    opcodeInfo[0x0A] = "LD A from (BC)";
    opcodeInfo[0x0B] = "DEC BC";
    opcodeInfo[0x0C] = "INC C";
    opcodeInfo[0x0D] = "DEC C";
    opcodeInfo[0x0E] = "LD C from (PC)";
    opcodeInfo[0x0F] = "RRCA";

    opcodeInfo[0x11] = "LD DE from (PC)";
    opcodeInfo[0x12] = "LD (DE) from A";
    opcodeInfo[0x13] = "INC DE";
    opcodeInfo[0x14] = "INC D";
    opcodeInfo[0x15] = "DEC D";
    opcodeInfo[0x16] = "LD D from (PC)";
    opcodeInfo[0x17] = "RLA";
    opcodeInfo[0x18] = "JR by i8 offset";
    opcodeInfo[0x09] = "ADD HL, DE";
    opcodeInfo[0x1A] = "LD A from (DE)";
    opcodeInfo[0x1B] = "DEC DE";
    opcodeInfo[0x1C] = "INC E";
    opcodeInfo[0x1D] = "DEC E";
    opcodeInfo[0x1E] = "LD E from (PC)";
    opcodeInfo[0x1F] = "RRA";
    opcodeInfo[0x20] = "JR by i8 offset if NZ";
    opcodeInfo[0x21] = "LD HL from (PC)";
    opcodeInfo[0x22] = "LD (HL++) from A";
    opcodeInfo[0x23] = "INC HL";
    opcodeInfo[0x24] = "INC H";
    opcodeInfo[0x25] = "DEC H";
    opcodeInfo[0x26] = "LD H from (PC)";

    opcodeInfo[0x28] = "JR by i8 offset if Z";
    opcodeInfo[0x29] = "ADD HL, HL";
    opcodeInfo[0x2A] = "LD A from (HL++)";
    opcodeInfo[0x2B] = "DEC HL";
    opcodeInfo[0x2C] = "INC L";
    opcodeInfo[0x2D] = "DEC L";
    opcodeInfo[0x2E] = "LD L from (PC)";
    opcodeInfo[0x2F] = "CPL";
    opcodeInfo[0x30] = "JR by i8 offset if NC";
    opcodeInfo[0x31] = "LD SP from (PC)";
    opcodeInfo[0x32] = "LD (HL--) from A";
    opcodeInfo[0x33] = "INC SP";
    opcodeInfo[0x34] = "INC (HL)";
    opcodeInfo[0x35] = "DEC (HL)";
    opcodeInfo[0x36] = "LD (HL) from (PC)";

    opcodeInfo[0x38] = "JR by i8 offset if C";
    opcodeInfo[0x39] = "ADD HL, SP";

    opcodeInfo[0x3B] = "DEC SP";
    opcodeInfo[0x3C] = "INC A";
    opcodeInfo[0x3D] = "DEC A";
    opcodeInfo[0x3E] = "LD A from (PC)";

    opcodeInfo[0x40] = "LD B from B";
    opcodeInfo[0x41] = "LD B from C";
    opcodeInfo[0x42] = "LD B from D";
    opcodeInfo[0x43] = "LD B from E";
    opcodeInfo[0x44] = "LD B from H";
    opcodeInfo[0x45] = "LD B from L";
    opcodeInfo[0x46] = "LD B from (HL)";
    opcodeInfo[0x47] = "LD B from A";
    opcodeInfo[0x48] = "LD C from B";
    opcodeInfo[0x49] = "LD C from C";
    opcodeInfo[0x4A] = "LD C from D";
    opcodeInfo[0x4B] = "LD C from E";
    opcodeInfo[0x4C] = "LD C from H";
    opcodeInfo[0x4D] = "LD C from L";
    opcodeInfo[0x4E] = "LD C from (HL)";
    opcodeInfo[0x4F] = "LD C from A";
    opcodeInfo[0x50] = "LD D from B";
    opcodeInfo[0x51] = "LD D from C";
    opcodeInfo[0x52] = "LD D from D";
    opcodeInfo[0x53] = "LD D from E";
    opcodeInfo[0x54] = "LD D from H";
    opcodeInfo[0x55] = "LD D from L";
    opcodeInfo[0x56] = "LD D from (HL)";
    opcodeInfo[0x57] = "LD D from A";
    opcodeInfo[0x58] = "LD E from B";
    opcodeInfo[0x59] = "LD E from C";
    opcodeInfo[0x5A] = "LD E from D";
    opcodeInfo[0x5B] = "LD E from E";
    opcodeInfo[0x5C] = "LD E from H";
    opcodeInfo[0x5D] = "LD E from L";
    opcodeInfo[0x5E] = "LD E from (HL)";
    opcodeInfo[0x5F] = "LD E from A";
    opcodeInfo[0x60] = "LD H from B";
    opcodeInfo[0x61] = "LD H from C";
    opcodeInfo[0x62] = "LD H from D";
    opcodeInfo[0x63] = "LD H from E";
    opcodeInfo[0x64] = "LD H from H";
    opcodeInfo[0x65] = "LD H from L";
    opcodeInfo[0x66] = "LD H from (HL)";
    opcodeInfo[0x67] = "LD H from A";
    opcodeInfo[0x68] = "LD L from B";
    opcodeInfo[0x69] = "LD L from C";
    opcodeInfo[0x6A] = "LD L from D";
    opcodeInfo[0x6B] = "LD L from E";
    opcodeInfo[0x6C] = "LD L from H";
    opcodeInfo[0x6D] = "LD L from L";
    opcodeInfo[0x6E] = "LD L from (HL)";
    opcodeInfo[0x6F] = "LD L from A";
    opcodeInfo[0x70] = "LD (HL) from B";
    opcodeInfo[0x71] = "LD (HL) from C";
    opcodeInfo[0x72] = "LD (HL) from D";
    opcodeInfo[0x73] = "LD (HL) from E";
    opcodeInfo[0x74] = "LD (HL) from H";
    opcodeInfo[0x75] = "LD (HL) from L";

    opcodeInfo[0x77] = "LD (HL) from A";
    opcodeInfo[0x78] = "LD A from B";
    opcodeInfo[0x79] = "LD A from C";
    opcodeInfo[0x7A] = "LD A from D";
    opcodeInfo[0x7B] = "LD A from E";
    opcodeInfo[0x7C] = "LD A from H";
    opcodeInfo[0x7D] = "LD A from L";
    opcodeInfo[0x7E] = "LD A from (HL)";
    opcodeInfo[0x7F] = "LD A from A";
    opcodeInfo[0x80] = "ADD A, B";
    opcodeInfo[0x81] = "ADD A, C";
    opcodeInfo[0x82] = "ADD A, D";
    opcodeInfo[0x83] = "ADD A, E";
    opcodeInfo[0x84] = "ADD A, H";
    opcodeInfo[0x85] = "ADD A, L";
    opcodeInfo[0x86] = "ADD A, (HL)";
    opcodeInfo[0x87] = "ADD A, A";




    opcodeInfo[0x90] = "SUB A, B";
    opcodeInfo[0x91] = "SUB A, C";
    opcodeInfo[0x92] = "SUB A, D";
    opcodeInfo[0x93] = "SUB A, E";
    opcodeInfo[0x94] = "SUB A, H";
    opcodeInfo[0x95] = "SUB A, L";
    opcodeInfo[0x96] = "SUB A, (HL)";
    opcodeInfo[0x97] = "SUB A, A";
    opcodeInfo[0xA8] = "XOR A with B";
    opcodeInfo[0xA9] = "XOR A with C";
    opcodeInfo[0xAA] = "XOR A with D";
    opcodeInfo[0xAB] = "XOR A with E";
    opcodeInfo[0xAC] = "XOR A with H";
    opcodeInfo[0xAD] = "XOR A with L";
    // opcodeInfo[0xAE] = "XOR A with (HL)";
    opcodeInfo[0xAF] = "XOR A with A";
    opcodeInfo[0xB8] = "CP A, B";
    opcodeInfo[0xB9] = "CP A, C";
    opcodeInfo[0xBA] = "CP A, D";
    opcodeInfo[0xBB] = "CP A, E";
    opcodeInfo[0xBC] = "CP A, H";
    opcodeInfo[0xBD] = "CP A, L";
    opcodeInfo[0xBE] = "CP A, (HL)";
    opcodeInfo[0xBF] = "CP A, A";

    opcodeInfo[0xC1] = "POP stack to BC";

    opcodeInfo[0xC5] = "PUSH BC to stack";
    opcodeInfo[0xC6] = "ADD A, u8";


    opcodeInfo[0xC9] = "RET";

    opcodeInfo[0xCB] = "CB-prefixed opcode! See next line";

    opcodeInfo[0xCD] = "CALL (PC)";

    opcodeInfo[0xD0] = "SUB A, (PC)";
    opcodeInfo[0xD1] = "POP stack to DE";

    opcodeInfo[0xD5] = "PUSH DE to stack";

    opcodeInfo[0xE0] = "LD (0xFF00+u8) from A"; ///////////////////
    opcodeInfo[0xE1] = "POP stack to HL";
    opcodeInfo[0xE2] = "LD (0xFF00+C) from A";


    opcodeInfo[0xE5] = "PUSH HL to stack";

    // opcodeInfo[0xE8] = "ADD SP, i8";

    opcodeInfo[0xEA] = "LD (u16) from A";





    opcodeInfo[0xF0] = "LD A from (0xFF00+u8)";
    opcodeInfo[0xF1] = "POP stack to AF";
    opcodeInfo[0xF2] = "LD A from (0xFF00+C)";

    opcodeInfo[0xF5] = "PUSH AF to stack";

    // opcodeInfo[0xF8] = "LD ????;
    opcodeInfo[0xF9] = "LD SP from HL";
    opcodeInfo[0xFA] = "LD A from (u16)";
    opcodeInfo[0xFB] = "Enable interrupts";


    opcodeInfo[0xFE] = "CP A, u8";

    opcodeCBInfo = std::vector<std::string>(0x100, "");
    opcodeCBInfo[0x00] = "RLC B";
    opcodeCBInfo[0x01] = "RLC C";
    opcodeCBInfo[0x02] = "RLC D";
    opcodeCBInfo[0x03] = "RLC B";
    opcodeCBInfo[0x04] = "RLC H";
    opcodeCBInfo[0x05] = "RLC L";
    opcodeCBInfo[0x06] = "RLC (HL)";
    opcodeCBInfo[0x07] = "RLC A";
    opcodeCBInfo[0x08] = "RRC B";
    opcodeCBInfo[0x09] = "RRC C";
    opcodeCBInfo[0x0A] = "RRC D";
    opcodeCBInfo[0x0B] = "RRC B";
    opcodeCBInfo[0x0C] = "RRC H";
    opcodeCBInfo[0x0D] = "RRC L";
    opcodeCBInfo[0x0E] = "RRC (HL)";
    opcodeCBInfo[0x0F] = "RRC A";
    opcodeCBInfo[0x10] = "RL B";
    opcodeCBInfo[0x11] = "RL C";
    opcodeCBInfo[0x12] = "RL D";
    opcodeCBInfo[0x13] = "RL B";
    opcodeCBInfo[0x14] = "RL H";
    opcodeCBInfo[0x15] = "RL L";
    opcodeCBInfo[0x16] = "RL (HL)";
    opcodeCBInfo[0x17] = "RL A";
    opcodeCBInfo[0x18] = "RR B";
    opcodeCBInfo[0x19] = "RR C";
    opcodeCBInfo[0x1A] = "RR D";
    opcodeCBInfo[0x1B] = "RR B";
    opcodeCBInfo[0x1C] = "RR H";
    opcodeCBInfo[0x1D] = "RR L";
    opcodeCBInfo[0x1E] = "RR (HL)";
    opcodeCBInfo[0x1F] = "RR A";



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
    if (!memoryMap.loadBootProgram(".//input//dmg_boot.gb")){
        return EXIT_FAILURE;
    }
    std::cout << "Loaded boot program\n";
    if (!memoryMap.loadCartridge(".//input//tetris.gb")){
        // return EXIT_FAILURE;
    }
    else{
        std::cout << "Loaded cartridge\n";
    }
    
    if (verbose){
        std::cout << "Encountered opcodes:";
    }
    while(executeNextOpcode()){

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
                        printRecentOpcodes();
                        std::cout << "\n\nPC at exit: 0x" << std::hex << PC << "\n";
                        return EXIT_SUCCESS;
                    }
                    break;
                default: break;
            }
        }


    }
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

uint16_t CPU::executeNextOpcode(){
    if (PC == 0x100){
        memoryMap.finishBooting();
    }
    if (false){
        printRecentOpcodes();
        return 0;
    }
    if (halted){
        return NOP();
    }
    else{
        uint8_t opcode = memoryMap.readByte(PC++);
        addOpcodeToLog(opcode);
        return executeOpcode(opcode);
    }
}

uint16_t CPU::executeOpcode(uint8_t opcode){
    if (verbose){
        std::cout << "\n";
        // std::cout << "HL: " << HL << ", ";
        printOpcode(opcode);
        std::cout << ": ";
        printOpcodeInfo(opcode);
    }
    switch(opcode){
    case 0x00: return NOP(); break;
    case 0x01: return LDrru16(BC); break;
    case 0x02: return LDnnr(BC, A); break;
    case 0x03: return INCrr(BC); break;
    case 0x04: return INCr(B); break;
    case 0x05: return DECr(B); break;
    case 0x06: return LDru8(B); break;
    case 0x07: return RLCA(); break;
    case 0x08: return LDu16rr(SP); break;
    case 0x09: return ADDrrrr(HL, BC); break;
    case 0x0A: return LDrnn(A, BC); break;
    case 0x0B: return DECrr(BC); break;
    case 0x0C: return INCr(C); break;
    case 0x0D: return DECr(C); break;
    case 0x0E: return LDru8(C); break;
    case 0x0F: return RRCA(); break;
    case 0x11: return LDrru16(DE); break;
    case 0x12: return LDnnr(DE, A); break;
    case 0x13: return INCrr(DE); break;
    case 0x14: return INCr(D); break;
    case 0x15: return DECr(D); break;
    case 0x16: return LDru8(D); break;
    case 0x17: return RLA(); break;
    case 0x18: return JRe(); break;
    case 0x19: return ADDrrrr(HL, DE); break;
    case 0x1A: return LDrnn(A, DE); break;
    case 0x1B: return DECrr(DE); break;
    case 0x1C: return INCr(E); break;
    case 0x1D: return DECr(E); break;
    case 0x1E: return LDru8(E); break;
    case 0x1F: return RRA(); break;
    case 0x20: return JRcce(FLAG_ZERO, false); break;
    case 0x21: return LDrru16(HL); break;
    case 0x22: return LDnnr(HL++, A); break;
    case 0x23: return INCrr(HL); break;
    case 0x24: return INCr(H); break;
    case 0x25: return DECr(H); break;
    case 0x26: return LDru8(H); break;

    case 0x28: return JRcce(FLAG_ZERO, true); break;
    case 0x29: return ADDrrrr(HL, HL); break;
    case 0x2A: return LDrnn(A, HL++); break;
    case 0x2B: return DECrr(HL); break;
    case 0x2C: return INCr(L); break;
    case 0x2D: return DECr(L); break;
    case 0x2E: return LDru8(L); break;
    case 0x2F: return CPL(); break;
    case 0x30: return JRcce(FLAG_CARRY, false); break;
    case 0x31: return LDrru16(SP); break;
    case 0x32: return LDnnr(HL--, A); break;
    case 0x33: return INCrr(SP); break;
    case 0x34: return INCnn(HL); break;
    case 0x35: return DECnn(HL); break;
    case 0x36: return LDrnn(A, HL--); break;

    case 0x38: return JRcce(FLAG_CARRY, true); break;
    case 0x39: return ADDrrrr(HL, SP); break;
    case 0x3A: return LDrnn(A, HL--); break;
    case 0x3B: return DECrr(SP); break;
    case 0x3C: return INCr(A); break;
    case 0x3D: return DECr(A); break;
    case 0x3E: return LDru8(A); break;

    case 0x40: return LDrr(B, B); break;
    case 0x41: return LDrr(B, C); break;
    case 0x42: return LDrr(B, D); break;
    case 0x43: return LDrr(B, E); break;
    case 0x44: return LDrr(B, H); break;
    case 0x45: return LDrr(B, L); break;
    case 0x46: return LDrnn(B, HL); break;
    case 0x47: return LDrr(B, A); break;
    case 0x48: return LDrr(C, B); break;
    case 0x49: return LDrr(C, C); break;
    case 0x4A: return LDrr(C, D); break;
    case 0x4B: return LDrr(C, E); break;
    case 0x4C: return LDrr(C, H); break;
    case 0x4D: return LDrr(C, L); break;
    case 0x4E: return LDrnn(C, HL); break;
    case 0x4F: return LDrr(C, A); break;
    case 0x50: return LDrr(D, B); break;
    case 0x51: return LDrr(D, C); break;
    case 0x52: return LDrr(D, D); break;
    case 0x53: return LDrr(D, E); break;
    case 0x54: return LDrr(D, H); break;
    case 0x55: return LDrr(D, L); break;
    case 0x56: return LDrnn(D, HL); break;
    case 0x57: return LDrr(D, A); break;
    case 0x58: return LDrr(E, B); break;
    case 0x59: return LDrr(E, C); break;
    case 0x5A: return LDrr(E, D); break;
    case 0x5B: return LDrr(E, E); break;
    case 0x5C: return LDrr(E, H); break;
    case 0x5D: return LDrr(E, L); break;
    case 0x5E: return LDrnn(E, HL); break;
    case 0x5F: return LDrr(E, A); break;
    case 0x60: return LDrr(H, B); break;
    case 0x61: return LDrr(H, C); break;
    case 0x62: return LDrr(H, D); break;
    case 0x63: return LDrr(H, E); break;
    case 0x64: return LDrr(H, H); break;
    case 0x65: return LDrr(H, L); break;
    case 0x66: return LDrnn(H, HL); break;
    case 0x67: return LDrr(H, A); break;
    case 0x68: return LDrr(L, B); break;
    case 0x69: return LDrr(L, C); break;
    case 0x6A: return LDrr(L, D); break;
    case 0x6B: return LDrr(L, E); break;
    case 0x6C: return LDrr(L, H); break;
    case 0x6D: return LDrr(L, L); break;
    case 0x6E: return LDrnn(L, HL); break;
    case 0x6F: return LDrr(L, A); break;
    case 0x70: return LDnnr(HL, B); break;
    case 0x71: return LDnnr(HL, C); break;
    case 0x72: return LDnnr(HL, D); break;
    case 0x73: return LDnnr(HL, E); break;
    case 0x74: return LDnnr(HL, H); break;
    case 0x75: return LDnnr(HL, L); break;

    case 0x77: return LDnnr(HL, A); break;
    case 0x78: return LDrr(A, B); break;
    case 0x79: return LDrr(A, C); break;
    case 0x7A: return LDrr(A, D); break;
    case 0x7B: return LDrr(A, E); break;
    case 0x7C: return LDrr(A, H); break;
    case 0x7D: return LDrr(A, L); break;
    case 0x7E: return LDrnn(A, HL); break;
    case 0x7F: return LDrr(A, A); break;
    case 0x80: return ADDrr(A, B); break;
    case 0x81: return ADDrr(A, C); break;
    case 0x82: return ADDrr(A, D); break;
    case 0x83: return ADDrr(A, E); break;
    case 0x84: return ADDrr(A, H); break;
    case 0x85: return ADDrr(A, L); break;
    case 0x86: return ADDrnn(A, HL); break;
    case 0x87: return ADDrr(A, A); break;





    case 0x90: return SUBrr(A, B); break;
    case 0x91: return SUBrr(A, C); break;
    case 0x92: return SUBrr(A, D); break;
    case 0x93: return SUBrr(A, E); break;
    case 0x94: return SUBrr(A, H); break;
    case 0x95: return SUBrr(A, L); break;
    case 0x96: return SUBrnn(A, HL); break;
    case 0x97: return SUBrr(A, A); break; 

    case 0xA8: return XORAr(B); break;
    case 0xA9: return XORAr(C); break;
    case 0xAA: return XORAr(D); break;
    case 0xAB: return XORAr(E); break;
    case 0xAC: return XORAr(H); break;
    case 0xAD: return XORAr(L); break;
    /* case 0xAE: return 0;//XORAnn(HL); break; */
    case 0xAF: return XORAr(A); break;


    case 0xB8: return CPrr(A, B); break;
    case 0xB9: return CPrr(A, C); break;
    case 0xBA: return CPrr(A, D); break;
    case 0xBB: return CPrr(A, E); break;
    case 0xBC: return CPrr(A, H); break;
    case 0xBD: return CPrr(A, L); break;
    case 0xBE: return CPrnn(A, HL); break;
    case 0xBF: return CPrr(A, A); break;

    case 0xC1: return POPrr(BC); break;

    case 0xC5: return PUSHrr(BC); break;
    case 0xC6: return ADDru8(A); break;


    case 0xC9: return RET(); break;

    case 0xCB: return executeCBOpcode(memoryMap.readByte(PC++)); break;

    case 0xCD: return CALLnn(); break;


    case 0xD1: return POPrr(DE); break;

    case 0xD5: return PUSHrr(DE); break;
    case 0xD6: return SUBru8(A); break;

    case 0xE0: return LDFFu8r(A); break;
    case 0xE1: return POPrr(HL); break;
    case 0xE2: return LDnnr(0xFF00 + C, A); break;

    case 0xE5: return PUSHrr(HL); break;

    // case 0xE8: ADD SP, i8 ???

    case 0xEA: return LDu16r(A); break;

    case 0xF0: return LDrFFu8(A);
    case 0xF1: return POPrr(AF); break;
    case 0xF2: return LDrnn(A, 0xFF00 + C); break;

    case 0xF5: return PUSHrr(AF); break;

    // case 0xF8????

    case 0xF9: return LDrrrr(SP, HL); break;
    case 0xFA: return LDru16(A); break;
    case 0xFB: return EI(); break;

    case 0xFE: return CPru8(A); break;

    default:     
        if (!verbose){
            std::cout << "Error: encountered unimplemented opcode\n\t...";
            printRecentOpcodes();   
        }
        std::cout << "unimplemented!\n";
        return 0;// throw; // Exceptions TBC
    break;
    }    
}

uint16_t CPU::executeCBOpcode(uint8_t opcode){
    addOpcodeToLog(opcode);
    if (verbose){
        std::cout << "\n";
        printOpcode(opcode);
        std::cout << ": ";
        printOpcodeCBInfo(opcode);
    }
    switch(opcode){
        // RLC/RRC
    case 0x00: return RLCr(B); break;
    case 0x01: return RLCr(C); break;
    case 0x02: return RLCr(D); break;
    case 0x03: return RLCr(B); break;
    case 0x04: return RLCr(H); break;
    case 0x05: return RLCr(L); break;
    // case 0x06: return RLCr((HL)); break;
    case 0x07: return RLCr(A); break;
    case 0x08: return RRCr(B); break;
    case 0x09: return RRCr(C); break;
    case 0x0A: return RRCr(D); break;
    case 0x0B: return RRCr(B); break;
    case 0x0C: return RRCr(H); break;
    case 0x0D: return RRCr(L); break;
    // case 0x0E: return RRCr((HL)); break;
    case 0x0F: return RRCr(A); break;
    // RL/RR
    case 0x10: return RLr(B); break;
    case 0x11: return RLr(C); break;
    case 0x12: return RLr(D); break;
    case 0x13: return RLr(B); break;
    case 0x14: return RLr(H); break;
    case 0x15: return RLr(L); break;
    // case 0x16: return RLr((HL)); break;
    case 0x17: return RLr(A); break;
    case 0x18: return RRr(B); break;
    case 0x19: return RRr(C); break;
    case 0x1A: return RRr(D); break;
    case 0x1B: return RRr(B); break;
    case 0x1C: return RRr(H); break;
    case 0x1D: return RRr(L); break;
    // case 0x1E: return RRr((HL)); break;
    case 0x1F: return RRr(A); break;
    case 0x40: return BITbr(0, B); break;
    case 0x41: return BITbr(0, C); break;
    case 0x42: return BITbr(0, D); break;
    case 0x43: return BITbr(0, E); break;
    case 0x44: return BITbr(0, H); break;
    case 0x45: return BITbr(0, L); break;
    // 0x46
    case 0x47: return BITbr(0, A); break;
    case 0x48: return BITbr(1, B); break;
    case 0x49: return BITbr(1, C); break;
    case 0x4A: return BITbr(1, D); break;
    case 0x4B: return BITbr(1, E); break;
    case 0x4C: return BITbr(1, H); break;
    case 0x4D: return BITbr(1, L); break;
    // 0x4E
    case 0x4F: return BITbr(1, A); break;
    case 0x50: return BITbr(2, B); break;
    case 0x51: return BITbr(2, C); break;
    case 0x52: return BITbr(2, D); break;
    case 0x53: return BITbr(2, E); break;
    case 0x54: return BITbr(2, H); break;
    case 0x55: return BITbr(2, L); break;
    // 0x56
    case 0x57: return BITbr(2, A); break;
    case 0x58: return BITbr(3, B); break;
    case 0x59: return BITbr(3, C); break;
    case 0x5A: return BITbr(3, D); break;
    case 0x5B: return BITbr(3, E); break;
    case 0x5C: return BITbr(3, H); break;
    case 0x5D: return BITbr(3, L); break;
    // 0x5E
    case 0x5F: return BITbr(3, A); break;
    case 0x60: return BITbr(4, B); break;
    case 0x61: return BITbr(4, C); break;
    case 0x62: return BITbr(4, D); break;
    case 0x63: return BITbr(4, E); break;
    case 0x64: return BITbr(4, H); break;
    case 0x65: return BITbr(4, L); break;
    // 0x66
    case 0x67: return BITbr(4, A); break;
    case 0x68: return BITbr(5, B); break;
    case 0x69: return BITbr(5, C); break;
    case 0x6A: return BITbr(5, D); break;
    case 0x6B: return BITbr(5, E); break;
    case 0x6C: return BITbr(5, H); break;
    case 0x6D: return BITbr(5, L); break;
    // 0x6E
    case 0x6F: return BITbr(5, A); break;
    case 0x70: return BITbr(6, B); break;
    case 0x71: return BITbr(6, C); break;
    case 0x72: return BITbr(6, D); break;
    case 0x73: return BITbr(6, E); break;
    case 0x74: return BITbr(6, H); break;
    case 0x75: return BITbr(6, L); break;
    // 0x76
    case 0x77: return BITbr(6, A); break;
    case 0x78: return BITbr(7, B); break;
    case 0x79: return BITbr(7, C); break;
    case 0x7A: return BITbr(7, D); break;
    case 0x7B: return BITbr(7, E); break;
    case 0x7C: return BITbr(7, H); break;
    case 0x7D: return BITbr(7, L); break;
    // 0x7E
    case 0x7F: return BITbr(7, A); break;
    default:
        if (!verbose){
            std::cout << "Error: encountered unimplemented CB opcode\n\t...";
            printRecentOpcodes();
        }
        std::cout << "unimplemented!\n";
        return 0;// throw; // Exceptions TBC
        break;
    }  

}

// NOP (0x00)
uint16_t CPU::NOP(){
    return 4; // add const??
}

// LDrru16 (0xN1, N = 0, 1, 2, 3)
// Loads word at (PC) to given register
uint16_t CPU::LDrru16(Register& targetReg){
    targetReg = readWordAtPC();
    return 12;
}

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

uint16_t CPU::LDFFu8r(HalfRegister& dataReg){
    memoryMap.writeByte(0xFF00 + readByteAtPC(), dataReg);
    return 12;
}

uint16_t CPU::LDru8(HalfRegister& targetReg){
    targetReg = readByteAtPC();
    return 8;
}

uint16_t CPU::LDu16rr(Register& dataReg){
    memoryMap.writeWord(readWordAtPC(), dataReg);
    return 20;
}

uint16_t CPU::LDu16r(HalfRegister& dataReg){
    memoryMap.writeByte(readWordAtPC(), dataReg);
    return 16;
}

uint16_t CPU::LDrr(HalfRegister& targetReg, HalfRegister& dataReg){
    targetReg = dataReg;
    return 4;
}

uint16_t CPU::LDrFFu8(HalfRegister& targetReg){
    targetReg = memoryMap.readByte(0xFF00 + readByteAtPC());
    return 12;
}

uint16_t CPU::LDrrrr(Register& targetReg, Register& dataReg){
    targetReg = dataReg;
    return 8;
}

uint16_t CPU::LDru16(HalfRegister& targetReg){
    targetReg = memoryMap.readByte(readWordAtPC());
    return 16;
}

// Pops top value of the stack to targetReg
// Flags altered iff targetReg is AF 
uint16_t CPU::POPrr(Register& targetReg){
    targetReg = memoryMap.readWord(SP);
    SP += 2;
    return 12;
}

uint16_t CPU::PUSHrr(Register& dataReg){
    SP -= 2;
    memoryMap.writeWord(SP, dataReg);
    return 16;
}

// XORAr (0xA8 - 0xAD, 0xAF)
// XORs A with given half register and stores result in A
uint16_t CPU::XORAr(HalfRegister reg){
    A ^= reg;
    setFlag(FLAG_ZERO, A == 0);
    clearFlag(FLAG_SUBTRACT);
    clearFlag(FLAG_HALFCARRY);
    clearFlag(FLAG_CARRY);
    return 4;
}

uint16_t CPU::INCrr(Register& reg){
    ++reg;
    return 8;
}

uint16_t CPU::INCr(HalfRegister& reg){
    setFlag(FLAG_HALFCARRY, (((reg & 0xF) + 1) & 0x10)/*  == 0x10 */);
    ++reg;
    setFlag(FLAG_ZERO, reg == 0);
    clearFlag(FLAG_SUBTRACT);
    return 4;
}

// 8-bit INC at address
uint16_t CPU::INCnn(uint16_t targetAddress){
    HalfRegister dummyReg{memoryMap.readByte(targetAddress)};
    INCr(dummyReg);
    memoryMap.writeByte(targetAddress, dummyReg);
    return 12;
}

uint16_t CPU::DECrr(Register& reg){
    --reg;
    return 8;
}

uint16_t CPU::DECr(HalfRegister& reg){
    setFlag(FLAG_HALFCARRY, ((reg & 0xF) - 1) & 0x10); // considered adding (...) == 0x10, but is redundant
    --reg;
    setFlag(FLAG_ZERO, reg == 0);
    setFlag(FLAG_SUBTRACT);
    return 4;
}

uint16_t CPU::DECnn(uint16_t targetAddress){
    HalfRegister dummyReg{memoryMap.readByte(targetAddress)};
    DECr(dummyReg);
    memoryMap.writeByte(targetAddress, dummyReg);
    return 12;
}

uint16_t CPU::ADDrrrr(Register& x, Register& y){
    setFlag(FLAG_CARRY, ((x & 0xFFFF) + (y & 0xFFFF)) & 0x10000);
    setFlag(FLAG_HALFCARRY, ((x & 0xFFF) + (y & 0xFFF)) & 0x1000);
    clearFlag(FLAG_SUBTRACT);
    x += y;
    return 8;
}

uint16_t CPU::ADDrr(HalfRegister& x, HalfRegister& y){
    setFlag(FLAG_CARRY, ((x & 0xFF) + (y & 0xFF)) & 0x100);
    setFlag(FLAG_HALFCARRY, ((x & 0xF) + (y & 0xF)) & 0x10);
    x += y;
    setFlag(FLAG_ZERO, x == 0);
    clearFlag(FLAG_SUBTRACT);
    return 4;
}

uint16_t CPU::ADDrnn(HalfRegister& x, uint16_t targetAddress){
    HalfRegister dummyReg{memoryMap.readByte(targetAddress)};
    ADDrr(x, dummyReg);
    return 8;
}

uint16_t CPU::ADDru8(HalfRegister& reg){
    HalfRegister dummyReg{readByteAtPC()};
    ADDrr(reg, dummyReg);
    return 8;
}

// Add SP i8???

uint16_t CPU::SUBrr(HalfRegister& x, HalfRegister& y){
    setFlag(FLAG_CARRY, ((x & 0xFF) - (y & 0xFF)) & 0x100);
    setFlag(FLAG_HALFCARRY, ((x & 0xF) - (y & 0xF)) & 0x10);
    x -= y;
    setFlag(FLAG_ZERO, x == 0);
    setFlag(FLAG_SUBTRACT);
    return 4;
}

uint16_t CPU::SUBrnn(HalfRegister& x, uint16_t targetAddress){
    HalfRegister dummyReg{memoryMap.readByte(targetAddress)};
    SUBrr(x, dummyReg);
    return 8;
}

uint16_t CPU::SUBru8(HalfRegister& reg){
    HalfRegister dummyReg{readByteAtPC()};
    SUBrr(reg, dummyReg);
    return 8;
}

uint16_t CPU::CPrr(HalfRegister& x, HalfRegister& y){
    setFlag(FLAG_CARRY, ((x & 0xFF) - (y & 0xFF)) & 0x100);
    setFlag(FLAG_HALFCARRY, ((x & 0xF) - (y & 0xF)) & 0x10);
    setFlag(FLAG_ZERO, x - y == 0);
    setFlag(FLAG_SUBTRACT);
    return 4;
}

uint16_t CPU::CPrnn(HalfRegister& x, uint16_t targetAddress){
    HalfRegister dummyReg{memoryMap.readByte(targetAddress)};
    CPrr(x, dummyReg);
    return 8;
}

uint16_t CPU::CPru8(HalfRegister& reg){
    HalfRegister dummyReg{readByteAtPC()};
    CPrr(reg, dummyReg);
    return 8;
}

uint16_t CPU::CPL(){
    A = ~A;
    setFlag(FLAG_SUBTRACT);
    setFlag(FLAG_HALFCARRY);
    return 4;
}

uint16_t CPU::CALLnn(){
    uint16_t nn = readWordAtPC();
    SP -= 2;
    memoryMap.writeWord(SP, PC);
    PC = nn;
    return 24;
}

uint16_t CPU::BITbr(uint8_t bit, HalfRegister reg){
    setFlag(FLAG_ZERO, !reg.testBit(bit));
    setFlag(FLAG_HALFCARRY);
    clearFlag(FLAG_SUBTRACT);
    return 8;
}

uint16_t CPU::BITbnn(uint8_t bit, uint16_t address){
    return BITbr(bit, memoryMap.readByte(address));
}

uint16_t CPU::EI(){
    interruptsEnabled = true;
    return 4;
}

uint16_t CPU::RLr(HalfRegister& reg){
    bool oldCarryState = isFlagSet(FLAG_CARRY);
    bool newCarryState = reg & (0x1 << 7);
    setFlag(FLAG_CARRY, newCarryState);
    C = C << 1;
    if (oldCarryState){
        reg |= 0x1;
    }
    else{
        reg &= ~0x1;
    }
    setFlag(FLAG_ZERO);
    clearFlag(FLAG_HALFCARRY);
    clearFlag(FLAG_SUBTRACT);
    return 8;
}

uint16_t CPU::RRr(HalfRegister& reg){
    bool oldCarryState = isFlagSet(FLAG_CARRY);
    bool newCarryState = reg & 0x1;
    setFlag(FLAG_CARRY, newCarryState);
    C = C >> 1;
    if (oldCarryState){
        reg |= (0x1 << 7);
    }
    else{
        reg &= int8_t(~(0x1 << 7));
    }
    setFlag(FLAG_ZERO);
    clearFlag(FLAG_HALFCARRY);
    clearFlag(FLAG_SUBTRACT);
    return 8;
}

uint16_t CPU::RLCr(HalfRegister& reg){
    bool newCarryState = reg & (0x1 << 7);
    C = C << 1;
    if (newCarryState){
        setFlag(FLAG_CARRY);
        reg |= 0x1;
    }
    else{
        clearFlag(FLAG_CARRY);
        reg &= ~0x1;
    }
    setFlag(FLAG_ZERO);
    clearFlag(FLAG_HALFCARRY);
    clearFlag(FLAG_SUBTRACT);
    return 8;
}

uint16_t CPU::RRCr(HalfRegister& reg){
    bool newCarryState = reg & 0x1;
    C = C >> 1;
    if (newCarryState){
        setFlag(FLAG_CARRY);
        reg |= (0x1 << 7);
    }
    else{
        clearFlag(FLAG_CARRY);
        reg &= int8_t(~(0x1 << 7));
    }
    setFlag(FLAG_ZERO);
    clearFlag(FLAG_HALFCARRY);
    clearFlag(FLAG_SUBTRACT);
    return 8;
}

uint16_t CPU::RLCA(){
    RLCr(A);
    clearFlag(FLAG_ZERO);
    return 4;
}

uint16_t CPU::RLA(){
    RLr(A);
    clearFlag(FLAG_ZERO);
    return 4;
}

uint16_t CPU::RRCA(){
    RRCr(A);
    clearFlag(FLAG_ZERO);
    return 4;
}

uint16_t CPU::RRA(){
    RRr(A);
    clearFlag(FLAG_ZERO);
    return 4;
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

uint16_t CPU::RET(){
    PC = memoryMap.readWord(SP);
    SP += 2;
    return 16;
}

uint8_t CPU::readByteAtPC(){
    return memoryMap.readByte(PC++); 
}

uint16_t CPU::readWordAtPC(){
    auto temp = memoryMap.readWord(PC);
    PC += 2;
    return temp;
}

void CPU::setFlag(uint8_t flag){
    F |= flag;
}

void CPU::setFlag(uint8_t flag, bool val){
    if (val){
        setFlag(flag);
    }
    else{
        clearFlag(flag);
    }
}

void CPU::clearFlag(uint8_t flag){
    F &= ~flag;
}

bool CPU::isFlagSet(uint8_t flag){
    return flag & F;
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

void CPU::addOpcodeToLog(uint8_t opcode){
    recentOpcodes.push_back(opcode);
    while (recentOpcodes.size() > maxOpcodeLookback){
        recentOpcodes.pop_front();
    }
}

void CPU::printRecentOpcodes(){
    bool lastOpcodeIsCBPrefix = false;
    for (auto op : recentOpcodes){
        std::cout << "\n\t";
        printOpcode(op);
        std::cout << ": ";
        if(lastOpcodeIsCBPrefix){
            std::cout << opcodeCBInfo[op];
            lastOpcodeIsCBPrefix = false;
        }
        else{
            std::cout << opcodeInfo[op];
            lastOpcodeIsCBPrefix = op == 0xCB;
        }
    }
}