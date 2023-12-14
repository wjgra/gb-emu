#ifndef _GB_EMU_EMULATOR_H_
#define  _GB_EMU_EMULATOR_H_

#include "..\inc\cpu.h"
#include "..\inc\gpu.h"
#include "..\inc\memory_map.h"

#include <string>
#include <chrono>

class GBEmulator final{
public:
    GBEmulator();
    bool start(std::string const& cartridgePath, std::string const& bootPath, bool printVerbose);
private:
    void finish();
    void frame();
    void handleEvents(SDL_Event const&  event);
    void updateTimers(uint16_t cycles);
    MemoryMap memoryMap;
    CPU cpu;
    GPU gpu;
    SDL_Window* window;
    uint32_t winFlags = SDL_WINDOW_SHOWN;
    unsigned int const winWidth = 160, winHeight = 144, winScale = 3;
    std::chrono::time_point<std::chrono::high_resolution_clock> tStart, tNow;
    unsigned int const maxFrameDuration = 250000;
    bool quit = false;
    uint32_t const maxClockFreq = 4194304; // Hz
    unsigned int cyclesSinceLastUpdate = 0;
    bool verbose = false;
    uint8_t directionInputReg, buttonInputReg;
};

#endif