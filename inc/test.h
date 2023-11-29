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
        {"Read (16-bit) register", testReadRegister},
        {"Read half (8-bit) register", testReadHalfRegister},
        {"Test bit of half register", testBitHalfRegister},
        {"Test register arithmetic/logical operations", testRegisterOps},
        {"Test half register arithmetic/logical operations", testHalfRegisterOps},
        {"Memory unit byte r/w", testByteRW},
        {"Memory uint word r/w", testWordRW}
    };
    // CPU/Register tests
    // bool testSystemEndianness();
    bool testReadRegister();
    bool testReadHalfRegister();
    bool testBitHalfRegister();
    bool testRegisterOps();
    bool testHalfRegisterOps();
    // Opcode tests


    // MMU tests
    bool testByteRW();
    bool testWordRW();
};

#endif

// Consider including a verbose option