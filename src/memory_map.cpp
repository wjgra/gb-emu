#include "..\inc\memory_map.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

MemoryMap::MemoryMap() : memory(0x10000, 0x00), bootMemory(0x100, 0x00){
}

uint8_t MemoryMap::readByte(uint16_t address){
    if (isBooting && (address < 0x0100)){
        return bootMemory[address];
    }
    else if (address >= 0xE000 && address < 0xFE00){
        // Echo RAM
        return memory[address - 0x2000];
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF){
        throw std::runtime_error("Access violation! Cannot read from [0xFEA0, 0xFEFF]");
    }
    else{
        return memory[address];
    }
}

uint16_t MemoryMap::readWord(uint16_t address){
    return  (readByte(address + 1) << 8) + readByte(address);
}

void MemoryMap::writeByte(uint16_t address, uint8_t value){
    if (address < 0x8000){
        throw std::runtime_error("Access violation! Cannot write to [0x0000, 0x7FFF]");
    }
    else if (address >= 0xE000 && address < 0xFE00){
        // Echo RAM
        memory[address - 0x2000] = value;
    }
    else if(address >= 0xFEA0 && address <= 0xFEFF){
        throw std::runtime_error("Access violation! Cannot write to [0xFEA0, 0xFEFF]");
    }
    else{
        memory[address] = value;
    }
}

void MemoryMap::writeByte(uint16_t address, uint8_t value, std::vector<uint8_t>& target){
    target[address] = value;
}

void MemoryMap::writeWord(uint16_t address, uint16_t value){
    writeByte(address + 1, value >> 8);
    writeByte(address, value & LOWER_BYTEMASK);
}

bool MemoryMap::loadBootProgram(std::string path){
    isBooting = true;
    return loadBinary(path, bootMemory);
}

bool MemoryMap::loadCartridge(std::string path){
    return loadBinary(path, memory);
}

bool MemoryMap::loadBinary(std::string path, std::vector<uint8_t>& target){
    std::ifstream fileStream(path.c_str(), std::ios_base::binary);
    if (!fileStream){
        return false;
    }
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
    for (std::size_t i = 0 ; i < data.size() ; ++i){
        writeByte(i, data[i], target);
    }
    return true;
}

void MemoryMap::finishBooting(){
    isBooting = false;
}