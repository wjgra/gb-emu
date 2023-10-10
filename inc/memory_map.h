#ifndef _GB_EMU_MEMORY_MAP_H_
#define  _GB_EMU_MEMORY_MAP_H_

#include <cstdint>
#include <array>
#include <fstream>
#include <iostream>
#include <iomanip>

class MemoryMap final{
public:
    MemoryMap();
    uint8_t readByte(uint16_t address);
    uint16_t readWord(uint16_t address);
    void writeByte(uint16_t address, uint8_t value);
    void writeWord(uint16_t address, uint16_t value);
    bool loadBootProgram(std::string path);
private:
    std::array<uint8_t, 0x10000> memory;
};

#endif