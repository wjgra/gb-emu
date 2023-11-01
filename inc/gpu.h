#ifndef _GB_EMU_GPU_H_
#define  _GB_EMU_GPU_H_

#include "..\inc\memory_map.h"
#include "..\inc\cpu.h"

#include <SDL.h>

class GPU{
public:
    GPU(MemoryMap& memMap, CPU& proc);
    void initialiseRenderer(SDL_Window* win);
    void update(uint16_t cycles);
    void render();
private:
    MemoryMap& memoryMap;
    CPU& cpu;
    SDL_Window* window;
    uint16_t clock;
    // LCD control bits
    bool LCDEnabled() const;
    bool windowTileMapArea() const;
    bool windowEnabled() const;
    bool bgWindowTileDataArea() const;
    bool bgTileMapArea() const;
    bool objSize() const;
    bool objEnabled() const;
    bool bgWindowEnabled() const;


    // ----
    uint8_t getMode() const;
    void setMode(uint8_t mode);
    void drawScanline();
    void drawBgScanline();
    void drawWindowScanline();
    void drawObjScanline();

    void pushFrame();

    void resetCurrentLine();
    uint8_t getCurrentLine();
    uint8_t incrementCurrentLine();

    // getters for scroll x,y ; win x, y

    uint16_t const cyclesPerLine = 456;
    uint16_t const scanlinesPerFrame = 154;
    uint16_t const vBlankDuration = 4560;
    uint16_t const hBlankDuration = 204;
    uint16_t const scanlineOAMDuration = 80;
    uint16_t const scanlineVRAMDuration = 172;
    uint16_t const linesInVBlank = 10;

    uint8_t const winWidth = 160;
    uint8_t const winHeight = 144;

    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> LCDtexture, bgBuffer, framebuffer; // Issue: consider renaming

    uint32_t const GB_COLOUR_BLACK = 0x00000000,
                   GB_COLOUR_DARK  = 0x606060FF,
                   GB_COLOUR_LIGHT = 0xC0C0C0FF,
                   GB_COLOUR_WHITE = 0xFFFFFFFF;
    std::array<uint32_t, 4> const colours = {GB_COLOUR_WHITE, 
                                             GB_COLOUR_LIGHT, 
                                             GB_COLOUR_DARK, 
                                             GB_COLOUR_BLACK};
};

#endif