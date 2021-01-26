#include "Instruction_Parser.h"

void ParseInstructions(const std::vector<std::string> &instructionsText,
                       std::vector<Instruction> &instructions)
{
    int instId = -1;
    int instArgType = -1;

    #define INSTRUCTION(name, id, argtype)         \
    {                                              \
        if (inst.find(#name) != std::string::npos) \
        {                                          \
            instId = id;                           \
            instArgType = argtype;                 \
        }                                          \
    }

    for (auto &inst : instructionsText)
    {
        #define INSTRUCTIONS
        #include "Commands_DSL.txt"

        #undef INSTRUCTIONS

        if (instId == -1)
        {
            std::cerr << "Error: Unidentified instruction {" << inst << "}\n";
            exit(EXIT_FAILURE);
        }

        instructions.push_back(ParseArguments(instId, instArgType, inst));

        int instId = -1;
        int instArgType = -1;
    }
#undef INSTRUCTION
}

Instruction ParseArguments(const int id, const int argType, std::string instructionText)
{    
    switch (argType)
    {
    case NOARG:
        return {id, argType};

    case LABEL:
        return {id, argType, instructionText.substr(instructionText.find(" ") + 1)};

    case NUMBER:
    {
        int number = 0;
        sscanf(instructionText.c_str(), "%*s %d", &number);
        return {id, argType, number};
    }

    case REG:
        return {id, argType, WhichReg(instructionText)};

    case REG_REG:
        return {id, argType, WhichReg(instructionText), WhichReg(instructionText, false)};

    case REG_NUMBER:
    {
        int number = 0;
        sscanf(instructionText.c_str(), "%*s %*s %d", &number);
        return {id, argType, WhichReg(instructionText), number};
    }
    }

    return {-1, -1};
}

int WhichReg(const std::string &instructionText, const bool isFirstArg)
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
        return RCX;

    default:
        std::cerr << "Error: Incorrect register {"
                  << registerText << "}\n";
        exit(EXIT_FAILURE);
    }
}