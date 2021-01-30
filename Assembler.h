#include <fstream>

#include "Instruction_Parser.h"

class Assembler 
{
private:
    std::string pathToInputFile_;
    std::string pathToOutputFile_ = "bytecode.txt";

    std::vector<std::string> instructionsText_;
    std::vector<Instruction> instructions_;
    std::map<std::string, OffsetLabel> labels_;

    void ReadFromFile();
    void ConvertToByteCode();

public:
    Assembler(const char* pathToInputFile) : pathToInputFile_(pathToInputFile) {}
    Assembler(const char *pathToInputFile, const char *pathToOutputFile) : 
            pathToInputFile_(pathToInputFile) , pathToOutputFile_(pathToOutputFile) {}

    void Assemble();

    void Dump() const;

    void SetOutputFile(char *const pathToOutputFile);

    ~Assembler() = default;
};