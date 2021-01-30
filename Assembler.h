#include <fstream>

#include "Instruction_Parser.h"

class Assembler 
{
private:
    std::vector<std::string> instructionsText_;
    std::vector<Instruction> instructions_;
    std::map<std::string, OffsetLabel> labels_;

    void ReadFromFile(char *const pathToFile);
    void ConvertToByteCode(char *const pathToOutputFile);

public:
    void Assemble(char *const pathToInputFile, char* const pathToOutputFile = "bytecode.txt");

    void Dump() const;
};