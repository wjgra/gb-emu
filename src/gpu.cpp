#include "..\inc\gpu.h"

GPU::GPU(MemoryMap& memMap, CPU& proc) : memoryMap{memMap}, cpu{proc}, clock{0}{
}

void GPU::setWindowPointer(SDL_Window* win){
    window = win;
}

void GPU::update(uint16_t cycles){
    // Set LCD status in 0xFF41

    // If LCD disabled, return
    if (LCDEnabled())
    {
        clock += cycles;
        /* while(clock >= scanlineOAMDuration) */{
            switch(getMode()){ // Issue: consider using enum for the four modes. Durations can be in an array
                // Horizontal blank
                case 0:
                    if (clock >= hBlankDuration){
                        clock -= hBlankDuration;
                        if (incCurrentLine() == winHeight){ // is this right? check panDocs
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
                        if (incCurrentLine() == winHeight + linesInVBlank){
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
    // Draw to screen
}

bool GPU::LCDEnabled(){
    HalfRegister temp = memoryMap.readByte(0xFF40);
    return temp.testBit(7);
}

uint8_t GPU::getMode(){
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
    // Push framebuffer to texture to be rendered to window by render()
}

void GPU::resetCurrentLine(){
    memoryMap.writeByte(0xFF44, 0);
}

uint8_t GPU::incCurrentLine(){
    HalfRegister temp = memoryMap.readByte(0xFF44);
    memoryMap.writeByte(0xFF44, ++temp);
    return temp;
}