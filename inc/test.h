#ifndef _GB_EMU_TEST_H_
#define  _GB_EMU_TEST_H_

#include "..\inc\emulator.h"
#include "..\inc\json.hpp"

#include <vector>
#include <algorithm>
#include <cmath>

class TestFramework final{
public:
    bool start();
private:
    struct BasicTest{
        std::string testName;
        bool (TestFramework::*run)();
    };
    std::vector<BasicTest> basicTests 
    {
        // {"System endianness", testSystemEndianness},
        {"Register (16-bit) r/w", testReadRegister},
        {"Half register (8-bit) r/w", testReadHalfRegister},
        {"Half register bit tests", testBitHalfRegister},
        {"Register arithmetic/logical operations", testRegisterOps},
        {"Half register arithmetic/logical operations", testHalfRegisterOps},
        {"Memory map byte r/w", testByteRW},
        {"Memory map word r/w", testWordRW}
    };
    // CPU/Register tests
    // bool testSystemEndianness();
    bool testReadRegister();
    bool testReadHalfRegister();
    bool testBitHalfRegister();
    bool testRegisterOps();
    bool testHalfRegisterOps();
    // MMU tests
    bool testByteRW();
    bool testWordRW();
    // Opcode tests
    bool testOpcode(uint8_t opcode, bool CBOpcode);
};

#endif

// Consider including a verbose option