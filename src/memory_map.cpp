#include "..\inc\memory_map.h"

uint16_t constexpr static UPPER_BYTEMASK = 0xFF00;
uint16_t constexpr static LOWER_BYTEMASK = 0x00FF;

MemoryMap::MemoryMap() : memory(0x10000, 0x00), bootMemory(0x100, 0x00), directionInputReg{0x00}, buttonInputReg{0x00}, 
    timer{.dividerCycles = timer.dividerFreq, .counterCycles = timer.counterFreq[timer.counterFreqIndex]} {
}

uint8_t MemoryMap::readByte(uint16_t address) const{
    if (disableMemMapping){
        return memory[address];
    }
    if (isBooting && (address < 0x0100)){
        return bootMemory[address];
    }
    else if (address >= 0xE000 && address < 0xFE00){
        // Echo RAM
        return memory[address - 0x2000];
    }
    else if (address >= 0xFEA0 && address <= 0xFEFF){
        // throw std::runtime_error("Access violation! Cannot read from [0xFEA0, 0xFEFF]");
        // Apparently this accessing region is meant to be a no-op (and similar for writeByte below)
        // Some games (including Tetris) use illegal reads/writes as a way to skip cycles!
        return 0x00;
    }
    else if (address == 0xFF00){
        // Process input
        HalfRegister inputSelection = memory[address];
        HalfRegister inputValue = 0x00;
        if(!inputSelection.testBit(4)){
            // D-pad enabled
            inputValue |= ~directionInputReg;
        }
        if(!inputSelection.testBit(5)){
            // Buttons enabled
            inputValue |= ~buttonInputReg;
        }
        return 0x3F & ((inputSelection | 0x0F) & inputValue);
    }
    else{
        return memory[address];
    }
}

uint16_t MemoryMap::readWord(uint16_t address) const{
    return  (readByte(address + 1) << 8) | readByte(address);
}

void MemoryMap::writeByte(uint16_t address, uint8_t value){
    if (disableMemMapping){
        memory[address] = value;
        return;
    }
    if (address < 0x8000){
        // throw std::runtime_error("Access violation! Cannot write to [0x0000, 0x7FFF]");
    }
    else if (address >= 0xE000 && address < 0xFE00){
        // Echo RAM
        memory[address - 0x2000] = value;
    }
    else if(address >= 0xFEA0 && address <= 0xFEFF){
        // throw std::runtime_error("Access violation! Cannot write to [0xFEA0, 0xFEFF]");
    }
    else if (address == 0xFF00){
        // Input register - only write to bits 4 and 5
        memory[address] = 0x30 & value; // lower bit info stored in inputRegs
    }
    else if (address == 0xFF04){
        // Attempting to write to divider register clears it
        memory[address] = 0;
    }
    else if (address == 0xFF07){
        // Only bottom three bits of timer control register are used
        memory[address] = value & 0x07;
        setCounterFrequency(value & 0x03);
    }
    else if (address == 0xFF46){
        // DMA transfer
        transferDMA(value);
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

bool MemoryMap::loadBootProgram(std::string const& path){
    isBooting = true;
    return loadBinary(path, bootMemory);
}

bool MemoryMap::loadCartridge(std::string const& path){
    return loadBinary(path, memory);
}

bool MemoryMap::loadBinary(std::string const& path, std::vector<uint8_t>& target){
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

bool MemoryMap::getBootStatus() const{
    return isBooting;
}

void MemoryMap::setState(std::vector<uint8_t> const& state){
    memory = state;
    finishBooting();
}

void MemoryMap::getState(std::vector<uint8_t>& state) const{
    state = memory;
}

void MemoryMap::disableMapping(bool disabled){
    disableMemMapping = disabled;
}

void MemoryMap::transferDMA(uint8_t value){
    uint16_t address = value << 8;
    for (int i = 0 ; i < 0xA0 ; ++i){
        writeByte(0xFE00 + i, readByte(address + i));
    }
}

// Return true to trigger timer interrupt
bool MemoryMap::updateTimerRegisters(uint16_t cycles){
    uint16_t const timerDividerAddress = 0xFF04;
    uint16_t const timerCounterAddress = 0xFF05;
    uint16_t const timerModuloAddress = 0xFF06;
    // uint16_t const timerControlAddress = 0xFF07;

    // step divider
    timer.dividerCycles -= cycles;
    while (timer.dividerCycles <= 0){
        timer.dividerCycles += timer.dividerFreq;
        incrementDIVRegister();
        if (readByte(timerDividerAddress) == 0x00){
            break; // break on overflow
        }
    }

    if (counterEnabled()){
        // step counter
        timer.counterCycles -= cycles;
        while (timer.counterCycles <= 0){
            timer.counterCycles += timer.counterFreq[timer.counterFreqIndex];
            incrementCounterRegister();
            if (readByte(timerCounterAddress) == 0x00){
                // overflow
                memory[timerCounterAddress] = memory[timerModuloAddress];
                return true; // request timer interrupt
            }
        }
    }
    return false;
}

void MemoryMap::incrementDIVRegister(){
    uint16_t const timerDividerAddress = 0xFF04;
    memory[timerDividerAddress]++;
}

void MemoryMap::incrementCounterRegister(){
    uint16_t const timerCounterAddress = 0xFF05;
    memory[timerCounterAddress]++;
}

bool MemoryMap::counterEnabled() const{
    uint16_t const timerControlAddress = 0xFF07;
    HalfRegister temp = readByte(timerControlAddress);
    return temp.testBit(2);
}

void MemoryMap::setCounterFrequency(uint8_t freq){
    timer.counterFreqIndex = freq;
    timer.counterCycles = timer.counterFreq[freq];
}

bool MemoryMap::processInput(uint8_t buttonInput, uint8_t directionInput){
    HalfRegister inputReg = readByte(0xFF00);
    uint8_t dirDelta = !inputReg.testBit(4) ? (directionInput ^ directionInputReg) & directionInputReg : 0x00;
    uint8_t butDelta = !inputReg.testBit(5) ? (buttonInput ^ buttonInputReg) & buttonInputReg : 0x00;
    buttonInputReg = buttonInput;
    directionInputReg = directionInput;
    return (dirDelta > 0x00) || (butDelta > 0x00); // if true, request joypad interrupt
}