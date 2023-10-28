#include <iostream>
#include <string>
#define SDL_MAIN_HANDLED
#include <SDL_main.h>

#include "..\inc\test.h"

int main(int argc, char** argv){
    try{
        if(argc > 1 && std::string(argv[1]) == "test"){
            Test test;
            return test.start();
        }
        else{
            GBEmulator emulator;
            return emulator.start();
        }
    }
    catch (const std::runtime_error& exception){
        std::cout << "\nException thrown: " << exception.what();
    }
}