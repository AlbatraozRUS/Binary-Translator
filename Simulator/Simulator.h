#ifndef BINARY_TRANSLATOR_SIMULATOR_SIMULATOR_H
#define BINARY_TRANSLATOR_SIMULATOR_SIMULATOR_H

#include "Constants.h"

#include <stack>

#include <iostream>

namespace BinaryTranslator {

class CpuSimulator {
private:
    int registers_[N_REGS] = {0};
    std::stack<int> stack_;
    std::stack<int> callerStack_;
    int isFlag = 0;

    size_t PC = 0;

    char* bytecode_ = nullptr;

    void ReadBytecode (char* const pathToInputFile);

    void AllocByteCodeBuf(size_t size);

public:
    CpuSimulator() = default;

    ~CpuSimulator()
    {
        delete[] bytecode_;
    }

    void Run(char *const pathToInputFile);

    void Dump() const;
}; //class CpuSimulator

}; //namespace BinaryTranslator

#endif // BINARY_TRANSLATOR_SIMULATOR_SIMULATOR_H