#ifndef BINARY_TRANSLATOR_ASSEMBLER_ASSEMBLER_H
#define BINARY_TRANSLATOR_ASSEMBLER_ASSEMBLER_H

#include "Instruction.h"

#include <vector>

namespace BinaryTranslator {

class Assembler {
private:
    std::string pathToInputFile_;
    std::string pathToOutputFile_;

    std::vector<std::string> instructionsText_;
    std::vector<Instruction> instructions_;
    std::map<std::string, OffsetLabel> labels_;

    void ReadFromFile();
    void ConvertToByteCode();

public:
    Assembler(const char* pathToInputFile, const char* pathToOutputFile);

    void Assemble();

    void Dump() const;

    ~Assembler();
}; // class Assembler

} // namespace BinaryTranslator

#endif // BINARY_TRANSLATOR_ASSEMBLER_ASSEMBLER_H