#include "..\inc\gpu.h"

uint32_t const static GB_COLOUR_BLACK = 0x00000000,
                      GB_COLOUR_DARK  = 0x606060FF,
                      GB_COLOUR_LIGHT = 0xC0C0C0FF,
                      GB_COLOUR_WHITE = 0xFFFFFFFF;
std::array<uint32_t, 4> const static colours = {GB_COLOUR_WHITE, GB_COLOUR_LIGHT, GB_COLOUR_DARK, GB_COLOUR_BLACK};

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

    // DMA??

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
                            cpu.requestInterrupt(1); // request LCD interrupt
                        }
                        else{
                            setMode(2);
                            cpu.requestInterrupt(1); // request LCD interrupt
                        }
                    }
                    break;
                // Vertical blank
                case 1:
                    if (clock >= cyclesPerLine){
                        clock -= cyclesPerLine;
                        if (incrementCurrentLine() == winHeight + linesInVBlank){
                            setMode(2);
                            resetCurrentLine();
                            cpu.requestInterrupt(1); // request LCD interrupt
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
                        cpu.requestInterrupt(1); // request LCD interrupt   
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

bool GPU::LCDEnabled() const{
    HalfRegister temp = memoryMap.readByte(LCDControlRegAddress);
    return temp.testBit(7);
}

// Determines which background map to use for window rendering
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
bool GPU::bgEnabled() const{
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
    if(windowEnabled() && bgEnabled()){
        drawWindowScanline();
    }
    // Copy current line of bg buffer into framebuffer
    std::copy(bgBuffer.begin() + winWidth * getCurrentLine(), bgBuffer.begin()  + winWidth * (getCurrentLine() + 1), framebuffer.begin() + winWidth * getCurrentLine());
    if(objEnabled()){
        drawObjScanline();
    }
}

void GPU::fillPalette(std::array<uint32_t, 4>& pal, uint16_t data){
    for (int i = 0 ; i < 4 ; ++i){
            pal[i] = colours[(data >> (2 * i)) & 0b11];
        }
}  

void GPU::drawBgScanline(){
    if(!bgEnabled()){
        // Draw all white pixels if bg disabled
        for (auto it = bgBuffer.begin() + winWidth * getCurrentLine(); it != bgBuffer.begin()  + winWidth * (getCurrentLine() + 1) ; ++it){
            *it = GB_COLOUR_WHITE;
        }
    }
    else{
        // Set addresses for the starts of the tile map and tile map data area respectively
        uint16_t const tileMapAddress = bgTileMapArea() ? 0x9C00 : 0x9800;
        uint16_t const tileMapDataAddress = bgWindowTileDataArea() ? 0x8000 : 0x9000;
        // Get tile y tile index and y pos within tile (same for all tiles in scanline)
        uint8_t const tileYIndex = ((getCurrentLine() + getScrollY()) / tileWidthInPixels) % tileMapWidth;
        uint8_t const tileYPos = (getCurrentLine() + getScrollY()) % tileWidthInPixels;
        // Load colour palette
        std::array<uint32_t, 4> palette;
        fillPalette(palette, getBgPalette());
/*         for (int i = 0 ; i < 4 ; ++i){
            palette[i] = colours[(getBgPalette() >> (2 * i)) & 0b11];
        } */
        // Draw each pixel in the scanline
        auto it = bgBuffer.begin() + winWidth * getCurrentLine();
        for (int x = 0 ; x < winWidth ; ++x, ++it){
            uint8_t const tileXIndex = ((x + getScrollX()) / tileWidthInPixels) % tileMapWidth;
            uint8_t const tileIndex = memoryMap.readByte(tileMapAddress + tileMapWidth * tileYIndex + tileXIndex);

            uint16_t tileDataAddress = tileMapDataAddress;
            if(bgWindowTileDataArea()){
                // Unsigned tile index (starting from 0x8000)
                tileDataAddress += tileIndex * tileSizeInBytes;
            }
            else{
                // Signed tile index (centred on 0x9000)
                tileDataAddress += static_cast<int8_t>(tileIndex) * tileSizeInBytes;
            }
            tileDataAddress += uint16_t(tileYPos * 2);
            HalfRegister b1 = memoryMap.readByte(tileDataAddress);
            HalfRegister b2 = memoryMap.readByte(tileDataAddress + 1);

            HalfRegister bit = 7 - ((getScrollX() + x) % 8);
            HalfRegister pLow = b1.testBit(bit) ? 0x01 : 0x00;
            HalfRegister pHigh = b2.testBit(bit) ? 0x02 : 0x00;

            *it = palette[pLow + pHigh];
        }
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

// Issue: consider combining below functions - possible enum?
/* get(ENUM){
    return readByte(LCDControlRegAddress + ENUM);
} */

uint8_t GPU::getScrollY() const{
    return memoryMap.readByte(LCDControlRegAddress + 2);
}

uint8_t GPU::getScrollX() const{
    return memoryMap.readByte(LCDControlRegAddress + 3);
}

void GPU::resetCurrentLine(){
    memoryMap.writeByte(LCDControlRegAddress + 4, 0);
}

uint8_t GPU::getCurrentLine() const{
    return memoryMap.readByte(LCDControlRegAddress + 4);
}

uint8_t GPU::incrementCurrentLine(){
    uint8_t temp = getCurrentLine();
    memoryMap.writeByte(LCDControlRegAddress + 4, ++temp);
    return temp;
}

// ...

uint8_t GPU::getBgPalette() const{
    return memoryMap.readByte(LCDControlRegAddress + 7);
}