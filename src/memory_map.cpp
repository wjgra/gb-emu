#include "..\inc\memory_map.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

MemoryMap::MemoryMap() : memory(0x10000, 0){
}

uint8_t MemoryMap::readByte(uint16_t address){
    return memory[address];
}

uint16_t MemoryMap::readWord(uint16_t address){
    return  (readByte(address + 1) << 8) + readByte(address);
}

void MemoryMap::writeByte(uint16_t address, uint8_t value){
    memory[address] = value;
}

void MemoryMap::writeWord(uint16_t address, uint16_t value){
    writeByte(address + 1, value >> 8);
    writeByte(address, value & LOWER_BYTEMASK);
}

bool MemoryMap::loadBootProgram(std::string path){
    std::ifstream fileStream(path.c_str(), std::ios_base::binary);
    if (!fileStream){
        return false;
    }
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
    for (std::size_t i = 0 ; i < data.size() ; ++i){
        writeByte(i, data[i]);
    }
    return true;
}