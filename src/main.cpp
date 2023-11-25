#include <iostream>
#include <string>
#include <cstring>
#define SDL_MAIN_HANDLED
#include <SDL_main.h>

#include "..\inc\test.h"

/* 
Command line arguments (can be used in any order, surplus args ignored)

-i [path]: set the path to the cartridge program

*OPTIONAL* -b [path]: set the path to the boot program

*OPTIONAL* -p: print ASCII chars output via serial port

*OPTIONAL* -test: run tests (ignores other args)
 */

int main(int argc, char** argv){
    try{
        std::vector<std::string> arguments(argv + 1, argv + argc);
        std::string cartridgePath, bootPath;
        bool printSerial = false;
        for(auto arg = arguments.begin() ; arg != arguments.end() ; ++arg){
            if (strcmp(arg->c_str(), "-t") == 0){
                Test test;
                return test.start();
            }
            else if (strcmp(arg->c_str(), "-i") == 0){
                if (arg != arguments.end() - 1){
                    ++arg;
                    cartridgePath = *arg;
                }
            }
            else if (strcmp(arg->c_str(), "-b") == 0){
                if (arg != arguments.end() - 1){
                    ++arg;
                    bootPath = *arg;
                }
            }
            else if (strcmp(arg->c_str(), "-p") == 0)
            {
                printSerial = true;
            }
        }
        GBEmulator emulator;
        return emulator.start(cartridgePath, bootPath, printSerial);  
    }
    catch (const std::runtime_error& exception){
        std::cout << "\nException thrown: " << exception.what();
    }

}