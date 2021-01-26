#include "CPU_Simulator.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: Incorrerct number of arguments\n";
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> instructionsText;
    ReadFromFile(argv[1], instructionsText);

    std::vector<Instruction> instructions;
    ParseInstructions(instructionsText, instructions);

    for (auto& inst : instructions)
        inst.Dump();

    return 0;
}

void ParseInstructions(const std::vector<std::string>& instructionsText, 
                             std::vector<Instruction>& instructions)
{
    int instId = -1;
    int instArgType = -1;
    
    #define INSTRUCTION(name, id, argtype)                    \
    {                                                         \
        if (inst.find(#name) != std::string::npos)            \
        {                                                     \
            instId = id;                                      \
            instArgType = argtype;                            \
        }                                                     \
    }

    for (auto& inst : instructionsText)
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
}

void ReadFromFile(char* pathToFile, std::vector<std::string>& instructionsText)
{
    std::ifstream inputFile(pathToFile);

    if (!inputFile)
    {
        std::cerr << "Error: Incorrect path to input file!\n";
        exit(EXIT_FAILURE);
    }

    std::string instructionText;

    while (getline(inputFile, instructionText, '\n'))
    {
        size_t posComment = instructionText.find_first_of("#");
        if (posComment != std::string::npos)
            instructionText.resize(posComment - 1);

            instructionsText.push_back(instructionText);
    }

    inputFile.close();
}

int WhichReg(const std::string& instructionText, const bool isFirstArg)
{
    std::string registerText;
    if (isFirstArg)
        registerText =
            instructionText.substr(instructionText.find_first_of("r"));
    else
        registerText =
            instructionText.substr(instructionText.find_last_of("r"));

    switch(registerText[1]){

        case 'a': return RAX;        
        case 'b': return RBX;
        case 'c': return RCX;
        case 'd': return RCX;

        default: std::cerr << "Error: Incorrect register {" 
                                << registerText << "}\n";
                 exit(EXIT_FAILURE);
    }
}