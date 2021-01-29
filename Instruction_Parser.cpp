#include "Instruction_Parser.h"

namespace {
int WhichReg(const std::string& instructionText, bool isFirstArg = true)
{
    char inputRegister[10] = {0};
    if (isFirstArg)
        sscanf(instructionText.c_str(), "%*s %[a-z 0-9]", inputRegister);
    else
        sscanf(instructionText.c_str(), "%*s %*s %s", inputRegister);

    for (auto kRegister : kRegisterList)
        if (inputRegister == kRegister.second)
            return kRegister.first;

    //TODO Change to exceptions
    std::cerr << "Error: Unidentified register {" << inputRegister << "}\n";
    exit(EXIT_FAILURE);
    }
};
//int WhichReg(const std::string &instructionText, const bool isFirstArg = true);

void Instruction::ParseInstruction(const std::string &instructionText)
{
    #define INSTRUCTION(name, id, argtype)         \
    {                                              \
        if (instructionText.find(#name) != std::string::npos) \
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

void Instruction::Dump() const
{    
    std::cout <<   "Id ["     << Id_    << "], ArgType [" << argType_ << "]\n" 
              << "\tArg_1 ["  << arg1_  << "] Arg_2 ["    << arg2_    << "]\n"
              << "\tlabel: {" << label_ << "}\n\n";   
}

std::string Instruction::Convert2ByteCode
        (const std::map<int, std::string>& labels, const int instPos) const
{
    std::string output;
    output += Id_;
    
    switch (argType_)
    {
    case NOARG:
        break;

    case REG_NUMBER:
    case REG_REG:
        output += arg2_;
    case NUMBER:
    case REG:
        output += arg1_;
        break;

    //TODO Fix labels to work in bytes. Двупроходный обход или считать байты по ходу команды
    case LABEL:
        int labelPos = -1;
        for (const auto &label : labels)
            if (label.second == label_)
                labelPos = label.first;

        if (labelPos == -1)
        {
            std::cerr << "Can`t find label {" << label_ << "}\n";
            exit(EXIT_FAILURE);
        }

        output += (labelPos - instPos);

        break;
    }    

    

    return output;  
}