#include "..\inc\test.h"

bool TestFramework::start(){
    // Run basic tests
    int numTestsPassed = 0;
    int const n = basicTests.size();
    int const testDisplayWidth = 50;
    int const nWidth = (int)std::log10(n);
    std::cout << "**BASIC TESTS**\n";
    for (int i = 0 ; i < n ; ++i){
        int const iWidth = (int)std::log10(i + 1);
        std::cout << "Test " << i + 1 << " of " << n << " (" << basicTests[i].testName << "):";
        int const padding = testDisplayWidth - nWidth - iWidth - basicTests[i].testName.length();
        for (int j = 0 ; j < padding ; ++j){
            std::cout << " ";
        }
        if ((this ->* (basicTests[i].run))()){
            std::cout << "PASSED\n";
            ++numTestsPassed;
        }
        else{
            std::cout << "FAILED\n";
        }
    }
    std::cout << "\n" << numTestsPassed << " out of " << n << " basic tests passed!\n";
    bool exitCode = numTestsPassed < n;

    // Run opcode tests - issue: consider splitting out into separate fn
    std::cout << "**OPCODE TESTS**";
    // Standard opcodes
    std::vector<int> illegalOpcodes{0x10 /*STOP*/, 0x76 /*HALT*/, 0xCB /*PREFIX*/, 0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD};
    int const numColumns = 8;
    numTestsPassed = 0;
    for (int i = 0x00 ; i < 0x100; ++i){
        std::cout << (((i % numColumns) == 0) ? "\n" : "\t") << "  0x";
        std::cout << std::setfill('0') << std::setw(2)
                  << std::hex << i << ": ";
        if (std::find(illegalOpcodes.begin(), illegalOpcodes.end(), i) != illegalOpcodes.end()){
            std::cout << "N/A";
        }
        else{
            bool result = testOpcode(i, false);
            if (result){
                ++numTestsPassed;
                std::cout << "PASSED";
            }
            else{
                std::cout << "FAILED";
            }
            exitCode = exitCode || result;
        }
    }
    std::cout << "\n";
    // CB opcodes
    for (int i = 0x00 ; i < 0x100; ++i){
        std::cout << (((i % numColumns) == 0) ? "\n" : "\t") << "0xcb";
        std::cout << std::setfill('0') << std::setw(2)
                  << std::hex << i << ": ";
        bool result = testOpcode(i, true);
        if (result){
            ++numTestsPassed;
            std::cout << "PASSED";
        }
        else{
            std::cout << "FAILED";
        }
        exitCode = exitCode || result;
    }
    std::cout << std::dec << "\n\n" << numTestsPassed << " out of " << (0x200 - illegalOpcodes.size()) << " opcode tests passed!\n";
    return exitCode;
}

bool TestFramework::testReadRegister(){
    Register reg(0x34, 0x12);
    return reg == 0x1234;
}

bool TestFramework::testReadHalfRegister(){
    Register reg(0x1234);
    return (reg.lowerByte == 0x34 && reg.upperByte == 0x12);
}

bool TestFramework::testByteRW(){
    MemoryMap memUnit;
    memUnit.writeByte(0xABCD, 0x56);
    return memUnit.readByte(0xABCD) == 0x56;
}

bool TestFramework::testWordRW(){
    MemoryMap memUnit;
    memUnit.writeWord(0xABCD, 0x5678);
    return memUnit.readWord(0xABCD) == 0x5678 &&
           memUnit.readByte(0xABCD) == 0x78 && 
           memUnit.readByte(0xABCD + 1) == 0x56;
}

bool TestFramework::testBitHalfRegister(){
    bool res = true;
    for (int i = 0 ; i < 8 ; ++i){
        HalfRegister reg{uint8_t(0b1 << i)};
        res = res && reg.testBit(i);
    }
    return res;
}

bool TestFramework::testRegisterOps(){
    bool res = true;
    uint16_t x = 0x1234, y = 0x5678;
    // +=
    Register temp{x};
    res = res && ((temp += y) == uint16_t(x + y));
    // -=
    temp = x;
    res = res && ((temp -= y) == uint16_t(x - y));
    /* // ^=
    temp = x;
    res = res && ((temp ^= y) == uint16_t(x ^ y));
    // |=
    temp = x;
    res = res && ((temp |= y) == uint16_t(x | y));
    // &=
    temp = x;
    res = res && ((temp &= y) == uint16_t(x & y)); */
    // -- (prefix)
    temp = x;
    res = res && ((--temp) == uint16_t(x - 1));
    res = res && (temp == uint16_t(x - 1));
    // -- (postfix)
    temp = x;
    res = res && ((temp--) == uint16_t(x));
    res = res && (temp == uint16_t(x - 1));
    // ++ (prefix)
    temp = x;
    res = res && ((++temp) == uint16_t(x + 1));
    res = res && (temp == uint16_t(x + 1));
    // ++ (postfix)
    temp = x;
    res = res && ((temp++) == uint16_t(x));
    res = res && (temp == uint16_t(x + 1));
    // Binary operations???
    return res;
}

bool TestFramework::testHalfRegisterOps(){
    bool res = true;
    uint8_t x = 0x12, y = 0x34;
    // ^=
    HalfRegister temp{x};
    res = res && ((temp ^= y) == uint8_t(x ^ y));
    // |=
    temp = x;
    res = res && ((temp |= y) == uint8_t(x | y));
    // &=
    temp = x;
    res = res && ((temp &= y) == uint8_t(x & y));
    // +=
    temp = x;
    res = res && ((temp += y) == uint8_t(x + y));
    // -=
    temp = x;
    res = res && ((temp -= y) == uint8_t(x - y));
    // -- (prefix)
    temp = x;
    res = res && ((--temp) == uint8_t(x - 1));
    res = res && (temp == uint8_t(x - 1));
    // -- (postfix)
    temp = x;
    res = res && ((temp--) == uint8_t(x));
    res = res && (temp == uint8_t(x - 1));
    // ++ (prefix)
    temp = x;
    res = res && ((++temp) == uint8_t(x + 1));
    res = res && (temp == uint8_t(x + 1));
    // ++ (postfix)
    temp = x;
    res = res && ((temp++) == uint8_t(x));
    res = res && (temp == uint8_t(x + 1));
    return res;
}

template <class iterator_type>
void stateFromJSON(iterator_type it, CPUState& result, std::string name){
    result.A = (*it)[name.c_str()]["a"].template get<uint8_t>();
    result.F = (*it)[name.c_str()]["f"].template get<uint8_t>();
    result.B = (*it)[name.c_str()]["b"].template get<uint8_t>();
    result.C = (*it)[name.c_str()]["c"].template get<uint8_t>(); 
    result.D = (*it)[name.c_str()]["d"].template get<uint8_t>();
    result.E = (*it)[name.c_str()]["e"].template get<uint8_t>();
    result.H = (*it)[name.c_str()]["h"].template get<uint8_t>();
    result.L = (*it)[name.c_str()]["l"].template get<uint8_t>();
    result.SP = (*it)[name.c_str()]["sp"].template get<uint16_t>();
    result.PC = (*it)[name.c_str()]["pc"].template get<uint16_t>(); 
    result.interrupts = false;
    result.halted = false;
    result.memory = std::vector<uint8_t>(0x10000, 0);
    auto it2 = (*it)[name.c_str()]["ram"].begin();
    for (; it2 != (*it)[name.c_str()]["ram"].end(); ++it2){
        result.memory[(*it2)[0].template get<uint16_t>()] = (*it2)[1].template get<uint8_t>();
    }
}

// Currently does not test 'interrupts' or 'halt'
// Update to handle interrupts (recall EI is 0xFFFF, while IME is interruptsEnabled)
bool TestFramework::testOpcode(uint8_t opcode, bool CBOpcode){
    std::stringstream path{};
    path << "tests\\";
    if (CBOpcode){
        path << "cb ";
    }
    path << std::setfill('0') << std::setw(2) 
         << std::hex << int(opcode) << ".json" << std::dec;
    std::ifstream testFile(path.str());
    if (!testFile){
        return false;
    }
    using json = nlohmann::json;
    json data = json::parse(testFile);
    auto it = data.begin();   
    bool res = true;
    for (auto it = data.begin(); it != data.end() ; ++it){
        CPUState initial, final, out;
        // Get initial and final test states
        stateFromJSON(it, initial, "initial");
        stateFromJSON(it, final, "final");
        // Configure and run test
        MemoryMap mem;
        CPU cpu(mem);
        mem.disableMapping(); // Tests assume a flat block of RAM
        cpu.setState(initial);
        uint16_t cycles = cpu.executeNextOpcode();
        // Compare output with final
        cpu.getState(out);
        out.interrupts = false; // Issue: currently does not test this
        out.halted = false; // Issue: currently does not test this
        res = res && (out == final);
        // Compare cycles
        res = res && (cycles == ((*it)["cycles"].size() * 4)); 
    }
    return res;
}  