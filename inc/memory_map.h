#ifndef _GB_EMU_MEMORY_MAP_H_
#define  _GB_EMU_MEMORY_MAP_H_

#include "..\inc\registers.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <string>

class MemoryMap final{
public:
    MemoryMap();
    uint8_t readByte(uint16_t address) const;
    uint16_t readWord(uint16_t address) const;
    void writeByte(uint16_t address, uint8_t value);
    void writeWord(uint16_t address, uint16_t value);
    bool loadBootProgram(std::string const& path);
    bool loadCartridge(std::string const& path);
    void finishBooting();
    bool getBootStatus() const;
    void setState(std::vector<uint8_t> const& state);
    void getState(std::vector<uint8_t>& state) const;
    void disableMapping(bool disabled = true);
    bool updateTimerRegisters(uint16_t cycles);
    bool processInput(uint8_t buttonInput, uint8_t directionInput);
private:
    void writeByte(uint16_t address, uint8_t value, std::vector<uint8_t>& target);
    bool loadBinary(std::string const& path, std::vector<uint8_t>& target);
    void transferDMA(uint8_t value);
    void incrementDIVRegister();
    void incrementCounterRegister();
    bool counterEnabled() const;
    void setCounterFrequency(uint8_t freq);
    std::vector<uint8_t> memory;
    std::vector<uint8_t> bootMemory;
    HalfRegister directionInputReg, buttonInputReg;
    struct Timer{
        int dividerCycles;
        uint16_t const dividerFreq = 0x100;
        int counterCycles;
        uint8_t counterFreqIndex = 0;
        std::array<uint16_t, 4> counterFreq = {0x400, 0x10, 0x40, 0x100};
    } timer;

    bool isBooting = false;
    bool disableMemMapping = false;
};

#endif