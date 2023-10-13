#ifndef _GB_EMU_TEST_H_
#define  _GB_EMU_TEST_H_

#include "..\inc\emulator.h"

#include <vector>
#include <cmath>

class Test final{
public:
    bool start();
private:
    struct UnitTest{
        std::string testName;
        bool (Test::*run)();
    };
    std::vector<UnitTest> tests 
    {
        // {"System endianness", testSystemEndianness},
        {"Read hybrid (16-bit) register", testReadHybridRegister},
        {"Read individual (8-bit) register", testReadIndivRegister},
        {"Memory unit byte r/w", testByteRW},
        {"Memory uint word r/w", testWordRW}
    };
    // CPU/Register tests
    // bool testSystemEndianness();
    bool testReadHybridRegister();
    bool testReadIndivRegister();
    // Opcode tests
    
    // MMU tests
    bool testByteRW();
    bool testWordRW();
};

#endif

// Consider including a verbose option