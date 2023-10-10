#include "..\inc\memory_map.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

MemoryMap::MemoryMap() : memory{}{
}

uint8_t MemoryMap::readByte(uint16_t address){
    return memory[address];
}

uint16_t MemoryMap::readWord(uint16_t address){
    return  (readByte(address) << 8) + readByte(address + 1);
}

void MemoryMap::writeByte(uint16_t address, uint8_t value){
    memory[address] = value;
}

void MemoryMap::writeWord(uint16_t address, uint16_t value){
    writeByte(address, value >> 8);
    writeByte(address + 1, value & LOWER_BYTEMASK);
}

bool MemoryMap::loadBootProgram(std::string path){
    std::ifstream fileStream(path.c_str());
    if (!fileStream){
        return false;
    }
    for (int i = 0 ; i < 0x10000 ; ++i){
        uint8_t thisByte;
        fileStream >> std::hex >> std::setw(2) >> thisByte;
        writeByte(i, thisByte);
    }
    return true;
}