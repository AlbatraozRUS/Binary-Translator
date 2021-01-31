#include "Assembler.h"

void Assembler::ReadFromFile()
{
    std::ifstream inputFile(pathToInputFile_, std::ios_base::in);

    if (!inputFile)
        throw std::runtime_error("Assemler: Invalid path to file\n");

    std::string instructionText;

    while (getline(inputFile, instructionText, '\n'))
    {        
        if (instructionText.find_first_of("#") != std::string::npos)
            continue;

        instructionsText_.push_back(instructionText);
    }

    inputFile.close();
}

void Assembler::Assemble()
{    
    ReadFromFile();
    
    std::string label;
    OffsetLabel temp = {0, 0};

    for (const auto &instText : instructionsText_)
    {
        if (instText[0] == ':')
        {
            label = instText.substr(1);
            labels_.insert(make_pair(label, temp));
        }
        else
        {
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
            if (!label.empty())
            {
                inst.SetLabeled(label);
                label.erase();
            }
            instructions_.push_back(inst);
        }
    }
    
    ConvertToByteCode();    
}

void Assembler::ConvertToByteCode()
{
    FILE *outputFile = fopen(pathToOutputFile_.c_str(), "wb");

    if (outputFile == nullptr)
        throw std::runtime_error("Assembler: Can`t create output file");

    std::string output;
    size_t offset = 0;

    for (const auto& inst : instructions_)
    {
        offset = output.size();

        if (!inst.GetLabeled().empty())
            labels_.at(inst.GetLabeled()).to = offset + 1;

        output += inst.Convert2ByteCode(labels_, offset);
    }

    for (const auto &inst : instructions_)
    {
        if (inst.GetArgType() == LABEL)
        {
            OffsetLabel offsetLabel = labels_.at(inst.GetLabel());
            output[offsetLabel.from] = offsetLabel.to - offsetLabel.from;
        }
    }

    fwrite(output.c_str(), 1, output.size(), outputFile);

    fclose(outputFile);
}

void Assembler::Dump() const 
{
    std::cerr << "\n-------------------------------------------------------------------------\n";
    std::cerr << "#[DUMP of Assembler]\n\n";

    std::cerr << "\n#[Parser_Begin]\n";
    for (const auto& inst : instructions_)
        inst.Dump();
    std::cerr << "#[Parser_End]\n";

    std::cerr << "\n#[Labels_Begin]\n";
    for (const auto& label : labels_)
        std::cerr << "\tLabel {" << label.first << "}\n"
        "\t\tFrom - " << label.second.from << ", to - " << label.second.to << "\n";
    std::cerr << "#[Labels_End]\n";

    std::cerr << "\n-------------------------------------------------------------------------\n";
}
