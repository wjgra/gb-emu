#ifndef _GB_EMU_TEST_H_
#define  _GB_EMU_TEST_H_

#include "..\inc\emulator.h"

#include <vector>
#include <cmath>

class Test{
public:
    bool start();
private:
    struct UnitTest{
        std::string testName;
        bool (Test::*run)();
    };
    std::vector<UnitTest> tests 
    {
        {"System endianness", testSystemEndianness},
        {"Read hybrid (16-bit) register", testReadHybridRegister},
        {"Read individual (8-bit) register", testReadIndivRegister},
    };
    // CPU/Register tests
    bool testSystemEndianness();
    bool testReadHybridRegister();
    bool testReadIndivRegister();
    // Opcode tests
    
    // MMU tests
};

#endif

// Consider including a verbose option