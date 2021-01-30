#include "Simulator.h"

void CpuSimulator::Run(char* const pathToInputFile)
{
    ReadBytecode(pathToInputFile);

    #define INSTRUCTION(name, num, argType, code){  \
        case num: {code; break;}                    \
    }

    #define INSTRUCTIONS
    while(true)
        switch(bytecode_[PC]){        
            #include "Commands_DSL.txt"
            default: throw std::runtime_error("Simulator: Unidentified instruction" + bytecode_[PC]);
        }

    #undef INSTRUCTIONS
    #undef INSTRUCTION    
}


void CpuSimulator::ReadBytecode (char* const pathToInputFile)
{
    FILE* inputFile = fopen(pathToInputFile, "rb");

    if (!inputFile)
        throw std::runtime_error("Simulator: Can`t open input file");

    fseek(inputFile, 0, SEEK_END);
    size_t fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    AllocByteCodeBuf(fileSize);

    fread(bytecode_, 1, fileSize, inputFile);

    fclose(inputFile);
}

void CpuSimulator::AllocByteCodeBuf(size_t size)
{
    bytecode_ = new char[size];
}
