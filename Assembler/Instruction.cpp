
#include "Instruction.h"

#include "Constants.h"

#include <iostream>

using namespace BinaryTranslator;

namespace {

const std::map<int, std::string> kRegisterList = { {RAX, "rax"},
                                                   {RBX, "rbx"},
                                                   {RCX, "rcx"},
                                                   {RDX, "rdx"} };

int WhichReg(const std::string &instructionText, bool isFirstArg = true)
{
    char inputRegister[10] = {0};
    if (isFirstArg)
        sscanf(instructionText.c_str(), "%*s %[a-z 0-9]", inputRegister);
    else
        sscanf(instructionText.c_str(), "%*s %*s %s", inputRegister);

    for (auto kRegister : kRegisterList)
        if (inputRegister == kRegister.second)
            return kRegister.first;

    throw std::runtime_error("Assembler: Unidentified register " +
                             instructionText);
}

}; // Anonymos namespace

class Instruction::Impl {
private:
    int Id_ = -1;
    int argType_ = -1;

    int arg1_ = 0;
    int arg2_ = 0;
    std::string label_;
    std::string labeled_;

public:
    Impl() = default;

    void ParseArguments(const std::string &instructionText);

    friend class Instruction;

}; //class Instruction::Impl


Instruction::Instruction() : pImpl_(std::make_unique<Impl>()) {};

Instruction::~Instruction() = default;

Instruction::Instruction(Instruction &&) = default;

Instruction& Instruction::operator=(Instruction &&) = default;


void Instruction::Impl::ParseArguments(const std::string &instructionText)
{
    switch (argType_) {
    case NOARG:
        return;

    case LABEL:
        label_ = instructionText.substr(instructionText.find(" ") + 1);
        return;

    case NUMBER:
        sscanf(instructionText.c_str(), "%*s %d", &arg1_);
        return;

    case REG:
        arg1_ = WhichReg(instructionText);
        return;

    case REG_REG:
        arg1_ = WhichReg(instructionText);
        arg2_ = WhichReg(instructionText, false);
        return;

    case REG_NUMBER: {
        arg1_ = WhichReg(instructionText);
        sscanf(instructionText.c_str(), "%*s %*s %d", &arg2_);
        return;
    }
    }
}


void Instruction::ParseInstruction(const std::string &instructionText)
{
#define INSTRUCTION(name, id, argtype, num, size, code) {       \
        if (instructionText.find(#name) != std::string::npos) { \
            pImpl_->Id_ = id;                                   \
            pImpl_->argType_ = argtype;                         \
        }                                                       \
    }

#define INSTRUCTIONS
#include "Commands_DSL.txt"

#undef INSTRUCTIONS

    if (pImpl_->Id_ == -1)
        throw std::runtime_error("Assembler: Unidentified instruction" +
                                 instructionText);

    pImpl_->ParseArguments(instructionText);

#undef INSTRUCTION
}


void Instruction::Dump() const
{
    std::cout << "Id ["       << std::hex << pImpl_->Id_      << "],"
              << "ArgType ["              << pImpl_->argType_ << "]\n"
              << "\tArg_1 ["  << std::hex << pImpl_->arg1_    << "]"
              << "Arg_2 ["    << std::hex << pImpl_->arg2_    << "]\n"
              << "\tlabel: {"             << pImpl_->label_   << "}\n\n";
}


std::string Instruction::ConvertToByteCode(std::map<std::string,
                                                    OffsetLabel> &labels,
                                           int offset) const
{
    std::string output;
    output += pImpl_->Id_;

    switch (pImpl_->argType_) {
    case NOARG:
        break;

    case REG_NUMBER:
    case REG_REG:
        output += pImpl_->arg1_;
        output += pImpl_->arg2_;
        break;

    case NUMBER:
    case REG:
        output += pImpl_->arg1_;
        break;

    case LABEL:
        output += 1;
        labels.at(pImpl_->label_).froms.push_back(offset + 1);
        break;
    }

    return output;
}


int Instruction::GetArgType() const
{
    return pImpl_->argType_;
}

std::string Instruction::GetLabel() const
{
    return pImpl_->label_;
}

std::string Instruction::GetLabeled() const
{
    return pImpl_->labeled_;
}

void Instruction::SetLabeled(const std::string &labeled)
{
    pImpl_->labeled_ = labeled;
}