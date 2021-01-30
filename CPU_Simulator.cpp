#include "CPU_Simulator.h"

namespace {
void ConvertToByteCode(const std::vector<Instruction> &instructions,
                             std::map<std::string, OffsetLabel> &labels)
{
    FILE* outputFile = fopen("bytecode.txt", "wb");

    if (outputFile == nullptr)
    {
        std::cerr << "Error: Incorrect path to output file!\n";
        exit(EXIT_FAILURE);
    }    

    std::string output;
    size_t offset = 0;

    for (size_t nInst = 0; nInst < instructions.size(); nInst++){
        offset = output.size();
        
        if (!instructions[nInst].GetLabeled().empty())            
            labels.at(instructions[nInst].GetLabeled()).to = offset + 1;

        output += instructions[nInst].Convert2ByteCode(labels, offset);
        
    }

    for (const auto& inst : instructions)
    {        
        if (inst.GetArgType() == LABEL)
        {
            OffsetLabel offsetLabel = labels.at(inst.GetLabel());
            output[offsetLabel.from] = offsetLabel.to - offsetLabel.from;
        }
    }

    fwrite(output.c_str(), 1, output.size(), outputFile);

    fclose(outputFile);
}
};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: Incorrect number of arguments\n";
        exit(EXIT_FAILURE);
    }    

    std::vector<std::string> instructionsText;
    try
    {
        ReadFromFile(argv[2], instructionsText);
    }

    catch (std::exception& exception)
    {
        std::cerr << exception.what() + std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<Instruction> instructions;
    std::map<std::string, OffsetLabel> labels;
    std::string label;
    OffsetLabel temp = {0, 0};

    for (const auto& instText : instructionsText)
    {
        if (instText[0] == ':')
        {
            label = instText.substr(1);
            labels.insert(make_pair(label, temp));            
        }
        else {
            Instruction inst;
            try
            {
            inst.ParseInstruction(instText);
            }
            catch (std::exception &exception)
            {
                std::cerr << exception.what();
                exit(EXIT_FAILURE);
            }
            if (!label.empty()) {
                inst.SetLabeled(label);
                label.erase();                
            }
            instructions.push_back(inst);
        }
    }
        
    ConvertToByteCode(instructions, labels);

    std::cerr << "\n#[Parser_Begin]\n";
    for (const auto& inst : instructions)
        inst.Dump();
    std::cerr << "\n#[Parser_End]\n";

    std::cerr << "\n#[Labels_Begin]\n";
    for (const auto& label : labels)    
        std::cerr << "Label {" << label.first << "}\n"
        "\tFrom - " << label.second.from << ", to - " << label.second.to << "\n";    
    std::cerr << "\n#[Labels_End]\n";

    return 0;
}

void ReadFromFile(char* pathToFile, std::vector<std::string>& instructionsText)
{
    std::ifstream inputFile(pathToFile, std::ios_base::in);
    
    if (!inputFile)
        throw std::runtime_error("Invalid path to file\n");    

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