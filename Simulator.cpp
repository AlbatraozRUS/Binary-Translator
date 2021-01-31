#include "Simulator.h"
#include "Assembler.h"

void CpuSimulator::Run(char* const pathToInputFile)
{
    ReadBytecode(pathToInputFile);

    #define INSTRUCTION(name, num, argType, code)  \
        case num: /*Dump();*/ code break;              \
    

    #define INSTRUCTIONS
    while(true)
        switch(bytecode_[PC]){                        
            #include "Commands_DSL.txt"
            default:
                throw std::runtime_error
                    ("Simulator: Unidentified instruction" + std::to_string(bytecode_[PC]));
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

void CpuSimulator::Dump() const
{
    fprintf(stderr, "ID: %d, PC: %zu\n\t Arg_1: %d, Arg_2: %d\n",
        bytecode_[PC], PC, bytecode_[PC + 1], bytecode_[PC + 2]);

    //TODO Changes numbers to RAX...
    fprintf(stderr, "Registers: RAX - %d, RBX - %d, RCX - %d, RDX - %d\n\n",
        registers_[0], registers_[1], registers_[2], registers_[3]);
}