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
    std::string registerText;
    if (isFirstArg)
        registerText =
            instructionText.substr(instructionText.find_first_of("r"));
    else
        registerText =
            instructionText.substr(instructionText.find_last_of("r"));

    switch (registerText[1])
    {

    case 'a':
        return RAX;
    case 'b':
        return RBX;
    case 'c':
        return RCX;
    case 'd':
        return RDX;

    default:
        std::cerr << "Error: Incorrect register {"
                  << registerText << "}\n";
        exit(EXIT_FAILURE);
    }
}

void Instruction::Dump()
{    
    std::cout << "Id [" << Id_ << "], ArgType [" << argType_ << "]\n" 
              <<  "\tArg_1 [" << arg1_ << "] Arg_2 [" << arg2_ << "]\n"
              << "\tlabel: {" << label_ << "}\n\n";   
}