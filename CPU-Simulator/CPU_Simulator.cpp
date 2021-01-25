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

    for (auto temp: instructionsText)
        std::cout << temp;

    std::vector<Instruction> instructions;
    ParseInstructions(instructionsText, instructions);


    return 0;
}

void ParseInstructions(const std::vector<std::string>& instructionsText, 
                       std::vector<Instruction>& instructions)
{

    for (auto& inst : instructionsText)
    {
        //TODO Think about another way of determining command, because current one sucks
        //Maybe the one i used in previous version
        if (CHECK_INST("exit"))
            instructions.push_back({EXIT});

        if (CHECK_INST("ret"))
            instructions.push_back({RET});

        if (CHECK_INST("push"))
        {
            int number = 0;
            if (sscanf(inst.c_str(), "%*s %d", &number))
                instructions.push_back({PUSH, number});
            else            
                instructions.push_back({PUSH_R, WhichReg(inst)});                                                
        }                         
        if (CHECK_INST("pop"))
            instructions.push_back({POP_R, WhichReg(inst)});   

        if (CHECK_INST("write"))
            instructions.push_back({WRITE, WhichReg(inst)});

        if (CHECK_INST("read"))
            instructions.push_back({READ, WhichReg(inst)});

        if (CHECK_INST("inc"))
            instructions.push_back({INC, WhichReg(inst)});

        if (CHECK_INST("dec"))
            instructions.push_back({DEC, WhichReg(inst)});
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

int WhichReg(const std::string& instructionText)
{
    std::string registerText = 
        instructionText.substr(instructionText.find_last_of("r"));

    switch(registerText[1]){

        case 'a': return RAX;        
        case 'b': return RBX;
        case 'c': return RCX;
        case 'd': return RCX;

        default: std::cerr << "Error: Incorrect register {" 
                                << registerText << "}\n";
                 abort();
    }
}