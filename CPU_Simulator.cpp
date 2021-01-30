#include "CPU_Simulator.h"

namespace {
void ConvertToByteCode(const std::vector<Instruction> &instructions,
                       const std::map<int, std::string> &labels)
{
    FILE* outputFile = fopen("bytecode.txt", "wb");

    if (outputFile == nullptr)
    {
        std::cerr << "Error: Incorrect path to output file!\n";
        exit(EXIT_FAILURE);
    }    

    std::string output;

    for (size_t nInst = 0; nInst < instructions.size(); nInst++)
        output += instructions[nInst].Convert2ByteCode(labels, nInst);

    fwrite(output.c_str(), 1, output.size(), outputFile);

    fclose(outputFile);
}
};

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
    for (const auto& instText : instructionsText)
    {
        if (instText[0] == ':')
        {
            labels.insert(make_pair(instructions.size() + 1, instText.substr(1)));
        }
        else {
            Instruction inst;
            inst.ParseInstruction(instText);
            instructions.push_back(inst);
        }
    }

    ConvertToByteCode(instructions, labels);

    // std::cerr << "\n#[Parser_Begin]\n";
    // for (auto& inst : instructions)
    //     inst.Dump();
    // std::cerr << "\n#[Parser_End]\n";

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