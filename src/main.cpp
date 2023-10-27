#include <iostream>
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL_main.h>

#include "..\inc\test.h"

int main(int argc, char** argv){

    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else{
        SDL_CreateWindow("GB-EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1, 1, SDL_WINDOW_SHOWN);
    }

    if(argc > 1 && std::string(argv[1]) == "test"){
        Test test;
        return test.start();
    }
    else{
        GBEmulator emulator;
        return emulator.start();
    }
}