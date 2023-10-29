#ifndef _GB_EMU_GPU_H_
#define  _GB_EMU_GPU_H_

#include "..\inc\memory_map.h"
#include "..\inc\cpu.h"

#include <SDL.h>

class GPU{
public:
    GPU(MemoryMap& memMap, CPU& proc);
    void setWindowPointer(SDL_Window* win);
    void update(uint16_t cycles);
    void render();
private:
    MemoryMap& memoryMap;
    CPU& cpu;
    SDL_Window* window;
    uint16_t clock;
    bool LCDEnabled();
    uint8_t getMode();
    void setMode(uint8_t mode);
    void drawScanline();
    void pushFrame();

    void resetCurrentLine();
    uint8_t incCurrentLine();

    uint16_t const cyclesPerLine = 456;
    uint16_t const scanlinesPerFrame = 154;
    uint16_t const vBlankDuration = 4560;
    uint16_t const hBlankDuration = 204;
    uint16_t const scanlineOAMDuration = 80;
    uint16_t const scanlineVRAMDuration = 172;
    uint16_t const linesInVBlank = 10;

    uint8_t const winWidth = 160;
    uint8_t const winHeight = 144;

};

#endif