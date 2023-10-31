#include "..\inc\gpu.h"

uint32_t const GB_COLOUR_BLACK = 0x00000000,
               GB_COLOUR_DARK  = 0x606060FF,
               GB_COLOUR_LIGHT = 0xC0C0C0FF,
               GB_COLOUR_WHITE = 0xFFFFFFFF;

GPU::GPU(MemoryMap& memMap, CPU& proc) : memoryMap{memMap}, cpu{proc}, clock{0}{
}

void GPU::initialiseRenderer(SDL_Window* win){
    window = win;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) throw std::runtime_error("Failed to create SDL renderer");

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, winWidth, winHeight);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    if (!texture) throw std::runtime_error("Failed to create SDL texture");

    LCDtexture = std::vector<uint32_t>(winHeight * winWidth, GB_COLOUR_WHITE);
    framebuffer = LCDtexture;

}

void GPU::update(uint16_t cycles){
    if (LCDEnabled())
    {
        clock += cycles;
        /* while(clock >= scanlineOAMDuration) */{
            switch(getMode()){ // Issue: consider using enum for the four modes. Durations can be in an array
                // Horizontal blank
                case 0:
                    if (clock >= hBlankDuration){
                        clock -= hBlankDuration;
                        if (getNextLine() == winHeight){ // is this right? check panDocs
                            setMode(1);
                            pushFrame();
                            cpu.requestInterrupt(0); // Issue: enum also required
                            // request LCD interrupt
                        }
                        else{
                            setMode(2);
                        }
                    }
                    break;
                // Vertical blank
                case 1:
                    if (clock >= vBlankDuration){
                        clock -= vBlankDuration;
                        if (getNextLine() == winHeight + linesInVBlank){
                            setMode(2);
                            resetCurrentLine();
                            // request LCD interrupt
                        }
                    }
                    break;
                // Scanline (accessing OAM)
                case 2:
                    if (clock >= scanlineOAMDuration){
                        clock -= scanlineOAMDuration;
                        setMode(3);
                    }
                    break;
                // Scanline (accessing VRAM)
                case 3:
                    if (clock >= scanlineVRAMDuration){
                        clock -= scanlineVRAMDuration;
                        setMode(0);
                        drawScanline();
                        // request LCD interrupt   
                    }
                    break;
                default:
                    throw std::runtime_error("Invalid GPU mode");
            }
        }
    }
}

void GPU::render(){
    // Draw LCDtexture to screen
    uint32_t *lockedPixels = nullptr; // needed?
    int pitch; // needed?
    SDL_RenderClear(renderer);
    SDL_LockTexture(texture, nullptr, (void **)&lockedPixels, &pitch);
    std::copy_n(LCDtexture.data(), LCDtexture.size(), lockedPixels);
    SDL_UnlockTexture(texture);

    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool GPU::LCDEnabled() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(7);
}

bool GPU::windowTileMapArea() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(6);
}

bool GPU::windowEnabled() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(5);
}

bool GPU::bgWindowTileDataArea() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(4);
}

bool GPU::bgTileMapArea() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(3);
}

bool GPU::objSize() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(2);
}

bool GPU::objEnable() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(1);
}

bool GPU::bgWindowEnable() const{
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(0);
}

uint8_t GPU::getMode() const{
    return memoryMap.readByte(0xFF41) & 0b11;
}

void GPU::setMode(uint8_t mode){
    HalfRegister temp = memoryMap.readByte(0xFF41);
    temp &= ~(0b11);
    temp += mode & 0b11;
    memoryMap.writeByte(0xFF41, temp);
}

void GPU::drawScanline(){
    // Draw a single line to framebuffer (does not get displayed until hBlank)


}

void GPU::pushFrame(){
    // Push framebuffer to LCDtexture to be rendered to window by render()
    LCDtexture = framebuffer;
}

void GPU::resetCurrentLine(){
    memoryMap.writeByte(0xFF44, 0);
}

uint8_t GPU::getNextLine(){
    HalfRegister temp = memoryMap.readByte(0xFF44);
    memoryMap.writeByte(0xFF44, ++temp);
    return temp;
}


// ScrollX: 0xFF42
// ScrollY: 0xFF43
// WindowY: 0xFF4A
// WindowX: 0xFF4B