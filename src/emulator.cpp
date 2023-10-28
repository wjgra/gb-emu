#include "..\inc\emulator.h"

GBEmulator::GBEmulator() : cpu{memoryMap}, gpu{memoryMap}{
}

bool GBEmulator::start(){
    window = SDL_CreateWindow("GB-EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, winFlags);
    gpu.setWindowPointer(window);

    if (!memoryMap.loadBootProgram(".//input//dmg_boot.gb")){
        throw std::runtime_error("Failed to load boot program");
    }
    std::cout << "Loaded boot program\n";
    if (!memoryMap.loadCartridge(".//input//tetris.gb")){
        throw std::runtime_error("Failed to load cartridge");
    }
    std::cout << "Loaded cartridge\n";
    
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        throw std::runtime_error("SDL failed to initialise (SDL error: " + std::string(SDL_GetError()) + ")");
    }
    
    tStart = std::chrono::high_resolution_clock::now();
    while (!quit){
        mainLoop();
    }
    return EXIT_SUCCESS;
}

void GBEmulator::mainLoop(){
    tNow = std::chrono::high_resolution_clock::now();
    unsigned int frameTime = std::chrono::duration_cast<std::chrono::microseconds>(tNow - tStart).count();
    if (frameTime > maxFrameDuration){
        frameTime = maxFrameDuration;
    }
    cpu.frame(frameTime);
    gpu.frame(frameTime);
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        handleEvents(event);
    }
    tStart = tNow;
}

void GBEmulator::handleEvents(SDL_Event const&  event){
    switch(event.type){
        case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
                cpu.stop();
                quit = true;
            }
            break;
        default:
            break;
    }
}