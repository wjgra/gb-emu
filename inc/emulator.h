#ifndef _GB_EMU_EMULATOR_H_
#define  _GB_EMU_EMULATOR_H_

#include "..\inc\cpu.h"
#include "..\inc\gpu.h"
#include "..\inc\memory_map.h"

#include <chrono>

class GBEmulator final{
public:
    GBEmulator();
    bool start();
    void mainLoop();
    void handleEvents(SDL_Event const&  event);
private:
    MemoryMap memoryMap;
    CPU cpu;
    GPU gpu;
    SDL_Window* window;
    uint32_t winFlags = SDL_WINDOW_SHOWN;
    unsigned int const winWidth = 160, winHeight = 144;
    std::chrono::time_point<std::chrono::high_resolution_clock> tStart, tNow;
    unsigned int maxFrameDuration = 250000;
    bool quit = false;
};

#endif