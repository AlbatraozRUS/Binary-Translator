#include "Instruction_Parser.h"

namespace {
    int WhichReg(const std::string instructionText, const bool isFirstArg);
};

void Instruction::ParseInstruction(const std::string &instructionText)
{
    #define INSTRUCTION(name, id, argtype)         \
    {                                              \
        if (inst.find(#name) != std::string::npos) \
        {                                          \
            Id_ = id;                              \
            argType_ = argtype;                    \
        }                                          \
    }
        
    #define INSTRUCTIONS
    #include "Commands_DSL.txt"

    #undef INSTRUCTIONS

    //TODO Change to exceptions
    if (Id_ == -1)
    {
        std::cerr << "Error: Unidentified instruction {" << instructionText << "}\n";
        exit(EXIT_FAILURE);
    }

    ParseArguments(instructionText);                

    #undef INSTRUCTION
}

void Instruction::ParseArguments(const std::string& instructionText)
{    
    switch (argType_)
    {
    case NOARG:
        return;

    case LABEL:
        label_ = instructionText.substr(instructionText.find(" ") + 1);
        return;

    case NUMBER:    
        sscanf(instructionText.c_str(), "%*s %d", &arg1_);
        return;

    case REG:
        arg1_ = WhichReg(instructionText);
        return;

    case REG_REG:
        arg1_ =  WhichReg(instructionText);
        arg2_ =  WhichReg(instructionText, false);
        return;

    case REG_NUMBER:
    {
        arg1_ = WhichReg(instructionText);
        sscanf(instructionText.c_str(), "%*s %*s %d", &arg2_);
        return;
    }
    }    
}

int WhichReg(const std::string instructionText, const bool isFirstArg)
{
    std::string inputRegister;
    sscanf(instructionText.c_str(), "%*s %[a-z 0-9]", inputRegister.c_str());

    const std::map<int, std::string> kRegisterList = {{RAX, "rax"}, {RBX, "rbx"},
                                                      {RCX, "rcx"}, {RDX, "rdx"}};

    for (auto kRegister : kRegisterList)
        if (inputRegister == kRegister.second)
            return kRegister.first;

    //TODO Change to exceptions        
    std::cerr << "Error: Unidentified register {" << inputRegister << "}\n";
    exit(EXIT_FAILURE);   
}

void Instruction::Dump() const
{    
    std::cout <<   "Id ["     << Id_    << "], ArgType [" << argType_ << "]\n" 
              << "\tArg_1 ["  << arg1_  << "] Arg_2 ["    << arg2_    << "]\n"
              << "\tlabel: {" << label_ << "}\n\n";   
}