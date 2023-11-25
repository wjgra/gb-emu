#include "..\inc\emulator.h"

GBEmulator::GBEmulator() : cpu{memoryMap}, gpu{memoryMap, cpu}{
}

bool GBEmulator::start(std::string const& cartridgePath, std::string const& bootPath, bool printSerial){
    window = SDL_CreateWindow("GB-EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winScale * winWidth, winScale * winHeight, winFlags);
    if (!window){
        throw std::runtime_error("Failed to create SDL window");
    }

    gpu.initialiseRenderer(window);

    if (bootPath.length() != 0){
        if (!memoryMap.loadBootProgram(bootPath)){
                throw std::runtime_error("Failed to load boot program at " + bootPath);
        }
        std::cout << "Loaded boot program\n";
    }
    else{
        cpu.simulateBoot();
        std::cout << "Simulated boot program execution\n";
    }
    
    if (cartridgePath.length() != 0){
        if (!memoryMap.loadCartridge(cartridgePath)){
            throw std::runtime_error("Failed to load cartridge at " + cartridgePath);
        }
        std::cout << "Loaded cartridge\n";
    }
    else{
        throw std::runtime_error("Specify cartridge path using '-i [PATH]'");
    }
    
    printSerialPort = printSerial;
    
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        throw std::runtime_error("SDL failed to initialise (SDL error: " + std::string(SDL_GetError()) + ")");
    }
    
    tStart = std::chrono::high_resolution_clock::now();
    while (!quit){
        frame();
    }
    return EXIT_SUCCESS;
}

void GBEmulator::finish(){
    cpu.finish();
    quit = true;
}

void GBEmulator::frame(){
    tNow = std::chrono::high_resolution_clock::now();
    unsigned int frameTime = std::chrono::duration_cast<std::chrono::microseconds>(tNow - tStart).count();
    if (frameTime > maxFrameDuration){
        frameTime = maxFrameDuration;
    }
    uint32_t maxCyclesThisFrame = uint32_t(maxClockFreq * frameTime);
    while (cyclesSinceLastUpdate < maxCyclesThisFrame){
        uint16_t cycles = cpu.executeNextOpcode();
        // update timers
        gpu.update(cycles);
        cpu.handleInterrupts(); // 5 M-cycles (per interrupt?)
        cyclesSinceLastUpdate += cycles;

        // Print serial data, if enabled
        if (printSerialPort && memoryMap.readByte(0xFF02) == 0x81){
            char c = memoryMap.readByte(0xFF01);
            printf("%c", c);
            memoryMap.writeByte(0xFF02, 0x00);
        }
    }
    cyclesSinceLastUpdate -= maxCyclesThisFrame;
    gpu.render();
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
                cpu.toggleHalt();
            }
            break;
        case SDL_QUIT:
            finish();
            break;
        default:
            break;
    }
}