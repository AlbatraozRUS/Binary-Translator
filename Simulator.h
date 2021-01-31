#include <stack>
#include <array>

#include <iostream>

class CpuSimulator {
private:
    int registers_[4] = {0};
    std::stack<int> stack_;
    std::stack<int> callerStack_;
    bool isFlag = false;

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
};