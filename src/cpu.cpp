#include "..\inc\cpu.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

CPU::CPU() : AF{}, BC{}, DE{}, HL{}, SP{}, PC{}{
}

bool CPU::start(){
    if (!memoryMap.loadBootProgram(".//input//dmg_boot.bin")){
        return EXIT_FAILURE;
    }
    executeNextOpcode();
    return EXIT_SUCCESS;
}

Register::Register() : upperByte{}, lowerByte{}{
}

Register::Register(uint8_t upperByte, uint8_t lowerByte) : upperByte{upperByte}, lowerByte{lowerByte}{
}

Register::Register(uint16_t val) : upperByte(val >> 8), lowerByte(val & LOWER_BYTEMASK){
}

Register::Register(Register const& other) : upperByte{other.upperByte}, lowerByte{other.lowerByte}{
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
    switch(opcode){
        case 0x00: 
            NOP();
            break;
        default:
            std::cout << "Encountered unimplemented opcode " << "0x" << std::setfill('0') << std::setw(2)
                      << std::hex << int(opcode) << "\n";
            throw; // Exceptions TBC
        break;
    }
    return 0;
}

// Flags
enum {
    flag_zero = 0x80,
    flag_subtract = 0x40,
    flag_halfCarry = 0x20,
    flag_carry = 0x19
};

// Instruction set (opcodes)

// NOP (0x00)
uint16_t CPU::NOP(){

    return 4;
}


    /* std::cout << "\n" << uint16_t(AF) << ", ";
    loadReg<&CPU::AF>(0x1234);
    std::cout << std::hex << "\n" << uint16_t(AF) << "\n"; */