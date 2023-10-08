#include <iostream>
#include <string>

#include "..\inc\test.h"

int main(int argc, char** argv){
    if(argc > 1 && std::string(argv[1]) == "test"){
        Test test;
        return test.start();
    }
    else{
        GBEmulator emulator;
        return emulator.start();
    }
}