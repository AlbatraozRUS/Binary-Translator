#include "CPU_Simulator.h"

int main(int argc, char **argv)
{
    // if (argc != 2)
    // {
    //     std::cerr << "Error: Incorrect number of arguments\n";
    //     exit(EXIT_FAILURE);
    // }    

    std::vector<std::string> instructionsText;
    ReadFromFile("test.txt", instructionsText);     //temp path because it is easier to debug))

    std::vector<Instruction> instructions;
    std::map<int, std::string> labels;
    ParseInstructions(instructionsText, instructions, labels);


    std::cerr << "\n#[Parser_Begin]\n";
    for (auto& inst : instructions)
        inst.Dump();
    std::cerr << "\n#[Parser_End]\n";

    return 0;
}

void ReadFromFile(char* pathToFile, std::vector<std::string>& instructionsText)
{
    std::ifstream inputFile(pathToFile, std::ios_base::in);

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