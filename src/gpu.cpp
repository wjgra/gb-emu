#include "..\inc\gpu.h"





GPU::GPU(MemoryMap& memMap, CPU& proc) : memoryMap{memMap}, cpu{proc}, clock{0}{
}

void GPU::initialiseRenderer(SDL_Window* win){
    window = win;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) throw std::runtime_error("Failed to create SDL renderer");

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, winWidth, winHeight);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    if (!texture) throw std::runtime_error("Failed to create SDL texture");

    LCDtexture = std::vector<uint32_t>(winHeight * winWidth, GB_COLOUR_BLACK);
    framebuffer = LCDtexture;
    bgBuffer = LCDtexture;

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
                        if (incrementCurrentLine() == winHeight){ // is this right? check panDocs
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
                        if (incrementCurrentLine() == winHeight + linesInVBlank){
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

// Memory addressses
// LCD Control: 0xFF40
// LCD Status: 0xFF41
// ScrollY: 0xFF42
// ScrollX: 0xFF43
// Current scanline (LY): 0xFF44 (read-only)
// LY Compare: 0xFF45
// ...
// BG palette: 0xFF47
// OBJ palettes: 0xFF48-49
// WindowY: 0xFF4A
// WindowX + 7: 0xFF4B

uint16_t const LCDControlRegAddress = 0xFF40;

bool GPU::LCDEnabled() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(7);
}

// Determines which background map used for window rendering
// Unset: 0x9800 tilemap used
// Set: 0x9C00 tilemap used
bool GPU::windowTileMapArea() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(6);
}

// Determines whether window is displayed
bool GPU::windowEnabled() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(5);
}

// Controls addressing mode for bg and window
// Unset ???
// Set: ???
bool GPU::bgWindowTileDataArea() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(4);
}

// Determines which map used for bg rendering
// Unset: 0x9800
// Set: 0x9C00
bool GPU::bgTileMapArea() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(3);
}

// Unset: ???
// Set: ???
bool GPU::objSize() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(2);
}

// Controls whether objects are displayed
bool GPU::objEnabled() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(1);
}

// Controls whether bg and window are displayed
bool GPU::bgWindowEnabled() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(0);
}

uint8_t GPU::getMode() const{
    return memoryMap.readByte(LCDControlRegAddress + 1) & 0b11;
}

void GPU::setMode(uint8_t mode){
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress + 1);
    temp &= ~(0b11);
    temp += mode & 0b11;
    memoryMap.writeByte(LCDControlRegAddress + 1, temp);
}

/*
There are 384 8x8 (16B) tiles stored at addresses 0x8000-0x97FF
Colours are 2-bit, with 0b00 == white, or transparent for objects (sprites)
We use two pixel buffers, one for the bg/window, and one for objects
*/

// Draw a single line to framebuffer (does not get displayed until hBlank)
// First draw bg and window to bgBuffer
// Then determine obj pixels, and composite with bgBuffer into framebuffer
void GPU::drawScanline(){
    drawBgScanline();
    if(windowEnabled() && bgWindowEnabled()){
        drawWindowScanline();
    }
    // Copy current line of bg buffer into framebuffer
    std::copy(bgBuffer.begin() + 160 * getCurrentLine(), bgBuffer.begin()  + winWidth * (getCurrentLine() + 1), framebuffer.begin() + winWidth * getCurrentLine());
    if(objEnabled()){
        drawObjScanline();
    }
}

void GPU::drawBgScanline(){
    // Draw all white pixels if bg disabled
    if(!bgWindowEnabled()){
        for (auto it = bgBuffer.begin() + 160 * getCurrentLine(); it != bgBuffer.begin()  + winWidth * (getCurrentLine() + 1) ; ++it){
            *it = GB_COLOUR_WHITE;
        }
    }
    else{
        // load palette

        // get tile map address (b/o mode)

        // find which tile to render

        // get pixel data from tile

        // copy corresponding colour to bgBuffer
    }
}

void GPU::drawWindowScanline(){

}

void GPU::drawObjScanline(){

}

void GPU::pushFrame(){
    // Push framebuffer to LCDtexture to be rendered to window by render()
    LCDtexture = framebuffer;
}

void GPU::resetCurrentLine(){
    memoryMap.writeByte(LCDControlRegAddress + 4, 0);
}

uint8_t GPU::getCurrentLine(){
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress + 4);
    return temp;
}

uint8_t GPU::incrementCurrentLine(){
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress + 4);
    memoryMap.writeByte(LCDControlRegAddress + 4, ++temp);
    return temp;
}