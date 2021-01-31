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
        
    throw std::runtime_error("Assembler: Unidentified register: " + instructionText);
}  
};

void Instruction::ParseInstruction(const std::string &instructionText)
{
    #define INSTRUCTION(name, id, argtype, code)   \
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
    
    if (Id_ == -1)
        throw std::runtime_error("Assembler: Unidentified instruction" + instructionText);
    
    ParseArguments(instructionText);                
    
    #undef INSTRUCTION
}

void Instruction::ParseArguments(const std::string& instructionText)
{    
    switch (argType_)
    {
    case NOARG:
        break;

    case LABEL:
        label_ = instructionText.substr(instructionText.find(" ") + 1);
        break;

    case NUMBER:    
        sscanf(instructionText.c_str(), "%*s %d", &arg1_);
        break;

    case REG:
        arg1_ = WhichReg(instructionText);
        break;

    case REG_REG:
        arg1_ =  WhichReg(instructionText);
        arg2_ =  WhichReg(instructionText, false);
        break;

    case REG_NUMBER:
    {
        arg1_ = WhichReg(instructionText);
        sscanf(instructionText.c_str(), "%*s %*s %d", &arg2_);
        break;
    }
    }    
}

std::string Instruction::Convert2ByteCode
        (std::map<std::string, OffsetLabel>& labels, const int offset) const
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

    case LABEL:
        output += 1;
        labels.at(label_).from = offset + 1;
        break;
    }        

    return output;  
}

void Instruction::Dump() const
{
    std::cout << "\tId [" << Id_ << "], ArgType [" << argType_ << "]\n"
              << "\t\tArg_1 [" << arg1_ << "] Arg_2 [" << arg2_ << "]\n"
              << "\t\tlabel: {" << label_ << "}\n\n";
}

int Instruction::GetArgType() const 
{
    return argType_;
}

std::string Instruction::GetLabel() const
{
    return label_;
}

std::string Instruction::GetLabeled() const
{
    return labeled_;
}

void Instruction::SetLabeled(const std::string& labeled)
{
    labeled_ = labeled;
}
