#ifndef BINARY_TRANSLATOR_ASSEMBLER_INSTRUCTION_H
#define BINARY_TRANSLATOR_ASSEMBLER_INSTRUCTION_H

#include <deque>
#include <experimental/propagate_const>
#include <map>
#include <memory>

namespace BinaryTranslator {

struct OffsetLabel {
    std::deque<size_t> froms;
    size_t to;
};

class Instruction {
private:
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl_;

public:
    Instruction();

    Instruction(const Instruction&) = delete;
    Instruction& operator=(const Instruction&) = delete;
    Instruction(Instruction&&);
    Instruction& operator=(Instruction&&);

    ~Instruction();

    void ParseInstruction(const std::string &instructionText);

    std::string ConvertToByteCode(std::map<std::string, OffsetLabel> &labels,
                                  int offset) const;

    int         GetArgType() const;
    std::string GetLabel()   const;
    std::string GetLabeled() const;

    void SetLabeled(const std::string &labeled);

    void Dump() const;

}; //class Instruction

} //namespace BinaryTranslator

#endif // BINARY_TRANSLATOR_ASSEMBLER_INSTRUCTION_H