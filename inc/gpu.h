#ifndef _GB_EMU_GPU_H_
#define  _GB_EMU_GPU_H_

#include "..\inc\memory_map.h"

#include <SDL.h>

class GPU{
public:
    GPU(MemoryMap& memMap);
    void setWindowPointer(SDL_Window* win);
    void frame(unsigned int frameTime);
    void render();
private:
    MemoryMap& memoryMap;
    SDL_Window* window;
};

#endif