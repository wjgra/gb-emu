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
            std::cout << "PASSED\n";
            ++numTestsPassed;
        }
        else{
            std::cout << "FAILED\n";
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

bool Test::testReadRegister(){
    Register reg(0x34, 0x12);
    return reg == 0x1234;
}

bool Test::testReadHalfRegister(){
    Register reg(0x1234);
    return (reg.lowerByte == 0x34 && reg.upperByte == 0x12);
}

bool Test::testByteRW(){
    MemoryMap memUnit;
    memUnit.writeByte(0xABCD, 0x56);
    return memUnit.readByte(0xABCD) == 0x56;
}

bool Test::testWordRW(){
    MemoryMap memUnit;
    memUnit.writeWord(0xABCD, 0x5678);
    return memUnit.readWord(0xABCD) == 0x5678 &&
           memUnit.readByte(0xABCD) == 0x78 && 
           memUnit.readByte(0xABCD + 1) == 0x56;
}

bool Test::testBitHalfRegister(){
    bool res = true;
    for (int i = 0 ; i < 8 ; ++i){
        HalfRegister reg{uint8_t(0b1 << i)};
        res = res && reg.testBit(i);
    }
    return res;
}

bool Test::testRegisterOps(){
    bool res = true;
    uint16_t x = 0x1234, y = 0x5678;
    // +=
    Register temp{x};
    res = res && ((temp += y) == uint16_t(x + y));
    // -=
    temp = x;
    res = res && ((temp -= y) == uint16_t(x - y));
    // ^=
    temp = x;
    res = res && ((temp ^ y) == uint16_t(x ^ y));
    // |=
    temp = x;
    res = res && ((temp | y) == uint16_t(x | y));
    // &=
    
    temp = x;
    res = res && ((temp & y) == uint16_t(x & y));
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

bool Test::testHalfRegisterOps(){
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

// Currently does not test 'interrupts' or 'halted'
bool Test::tempTest(){
    /* std::ifstream f("tests\\00.json");
    using json = nlohmann::json;
    json data = json::parse(f);
    auto it = data.begin(); */
   /*  CPUState in;stateFromJSON(it, in, "initial");
    

   
        CPUState final;
        stateFromJSON(it, final, "final");

    MemoryMap mem;
    mem.finishBooting();
    CPU cpu(mem);
    cpu.setState(in);
    uint16_t cycles = cpu.executeNextOpcode();
    CPUState out;
    cpu.getState(out);
    out.interrupts = false; //temp
    out.halted = false;
    bool res = out == final;
    res = res && (cycles == ((*it)["cycles"].size() * 4));
    return res; */

    std::ifstream f("tests\\03.json");
    using json = nlohmann::json;
    json data = json::parse(f);
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