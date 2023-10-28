#include "..\inc\gpu.h"

GPU::GPU(MemoryMap& memMap) : memoryMap{memMap}{
}

void GPU::setWindowPointer(SDL_Window* win){
    window = win;
}

void GPU::frame(unsigned int frameTime){

}

void GPU::render(){
    // Draw data updated in frame
}