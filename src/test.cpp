#include "..\inc\test.h"

bool Test::start(){
    int numTestsPassed = 0;
    int const n = tests.size();
    int const testDisplayWidth = 50;
    int const nWidth = (int)std::log10(n);
    for (int i = 0 ; i < n ; ++i){
        int const iWidth = (int)std::log10(i + 1);
        std::cout << "Test " << i + 1 << " of " << n << " (" << tests[i].testName << "):";
        int const padding = testDisplayWidth - nWidth - iWidth - tests[i].testName.length();
        for (int j = 0 ; j < padding ; ++j){
            std::cout << " ";
        }
        if ((this ->* (tests[i].run))()){
            std::cout << "FAILED\n";
        }
        else{
            std::cout << "PASSED\n";
            ++numTestsPassed;
        }
    }
    std::cout << "\n" << numTestsPassed << " out of " << n << " tests passed!\n";
    return numTestsPassed < n;
}

/* bool Test::testSystemEndianness(){
    uint16_t word = 0x1234;
    uint8_t* mLower = reinterpret_cast<uint8_t*>(&word);
    return (*mLower != 0x34 || *(mLower + 1) != 0x12);
} */

bool Test::testReadHybridRegister(){
    Register reg(0x34, 0x12);
    return reg != 0x1234;
}

bool Test::testReadIndivRegister(){
    Register reg(0x1234);
    return (reg.lowerByte != 0x34 || reg.upperByte != 0x12);
}

bool Test::testByteRW(){
    MemoryMap memUnit;
    memUnit.writeByte(0x1234, 0x56);
    return memUnit.readByte(0x1234) != 0x56;
}

bool Test::testWordRW(){
    MemoryMap memUnit;
    memUnit.writeWord(0x1234, 0x5678);
    return memUnit.readWord(0x1234) != 0x5678 || memUnit.readByte(0x1234) != 0x78 || memUnit.readByte(0x1234 + 1) != 0x56;
}