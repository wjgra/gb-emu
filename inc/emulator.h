#ifndef _GB_EMU_EMULATOR_H_
#define  _GB_EMU_EMULATOR_H_

#include "..\inc\cpu.h"
#include "..\inc\mmu.h"

class GBEmulator{
public:
    bool start();
private:
    CPU cpu;
};

#endif