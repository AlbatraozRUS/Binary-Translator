#include <fstream>
#include <vector>

#include "Instruction.h"

using namespace CPU_Simulator;

class Assembler 
{
private:
    std::string pathToInputFile_;
    std::string pathToOutputFile_;

    std::vector<std::string> instructionsText_;
    std::vector<Instruction> instructions_;
    std::map<std::string, OffsetLabel> labels_;

    void ReadFromFile();
    void ConvertToByteCode();

public:    
    Assembler(const char *pathToInputFile, const char *pathToOutputFile);

    void Assemble();

    void Dump() const;

    ~Assembler();
};