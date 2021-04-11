
#include "Assembler.h"

#include "Constants.h"

#include <fstream>
#include <iostream>

using namespace BinaryTranslator;

Assembler::Assembler(const char* pathToInputFile,
                     const char* pathToOutputFile) :
    pathToInputFile_(pathToInputFile),
    pathToOutputFile_(pathToOutputFile)
{}

Assembler::~Assembler() = default;

void Assembler::ReadFromFile()
{
    std::ifstream inputFile(pathToInputFile_, std::ios_base::in);

    if (!inputFile)
        throw std::runtime_error("Assembler: Invalid path to file\n");

    std::string instructionText;

    while (getline(inputFile, instructionText, '\n')) {
        if (instructionText.find_first_of("#") != std::string::npos)
            continue;

        if (!instructionText[0])
            continue;

        std::cerr << instructionText << "\n";
        instructionsText_.push_back(instructionText);
    }

    inputFile.close();
}

void Assembler::Assemble()
{
    ReadFromFile();

    std::string label;
    OffsetLabel temp;

    for (const auto& instText : instructionsText_) {
        if (instText[0] == ':') {
            label = instText.substr(1);
            labels_.insert(make_pair(label, temp));
        }
        else {
            Instruction inst;
            try {
                inst.ParseInstruction(instText);
            }
            catch (std::exception &exception) {
                std::cerr << exception.what();
                exit(EXIT_FAILURE);
            }
            if (!label.empty()) {
                inst.SetLabeled(label);
                label.erase();
            }
            instructions_.push_back(std::move(inst));
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

    for (const auto& inst : instructions_) {
        offset = output.size();

        if (!inst.GetLabeled().empty())
            labels_.at(inst.GetLabeled()).to = offset + 1;

        output += inst.ConvertToByteCode(labels_, offset);
    }

    for (const auto &inst : instructions_) {
        if (inst.GetArgType() == LABEL) {
            OffsetLabel& offsetLabel = labels_.at(inst.GetLabel());
            int offset = offsetLabel.to - offsetLabel.froms.front();
            if (offset > 128 || offset < -128)
                throw std::runtime_error("Assembler: too big jump");
            output[offsetLabel.froms.front()] = offset;
            offsetLabel.froms.pop_front();
        }
    }

    fwrite(output.c_str(), 1, output.size(), outputFile);

    fclose(outputFile);
}

void Assembler::Dump() const
{
    std::cerr << "\n--------------------------------------------------------\n";
    std::cerr << "#[DUMP of Assembler]\n\n";

    std::cerr << "\n#[Parser_Begin]\n";
    for (const auto& inst : instructions_)
        inst.Dump();
    std::cerr << "#[Parser_End]\n";

    std::cerr << "\n#[Labels_Begin]\n";
    for (const auto& label : labels_) {
        std::cerr << "\tLabel {" << label.first << "}\n"
                  << "\t\tTo: " << label.second.to << "\n"
                  << "\t\tFrom:\n";
        for (const auto from : label.second.froms)
            std::cerr << "\t\t" << from << "\n";
    }
    std::cerr << "#[Labels_End]\n";

    std::cerr << "\n--------------------------------------------------------\n";
}
