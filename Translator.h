#ifndef BINARY_TRANSLATOR_TRANSLATOR_H
#define BINARY_TRANSLATOR_TRANSLATOR_H

#include <experimental/propagate_const>
#include <memory>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class Translator {
private: 
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl_;

public:

    Translator(char *const pathToInputFile);

    Translator(const Translator &) = delete;
    Translator &operator=(const Translator &) = delete;
    Translator(Translator &&);

    ~Translator();

    void Translate();

    void Dump() const;
};

enum Id_Instructions
{
    PUSH = 0x68,
    PUSH_R = 0x50,
    POP_R = 0x58,
    MOV = 0xB8,
    MOV_R = 0xBB,
    CALL = 0xE8,
    RET = 0xC3,
    EXIT = 0xFF,
    WRITE = 0xE6,
    READ = 0xE4,

    ADD = 0x01,
    SUB = 0x29,
    IMUL = 0x6B,
    IDIV = 0xF6,
    ADD_R = 0x03,
    SUB_R = 0x2B,
    IMUL_R = 0xAF,
    IDIV_R = 0xF7,
    INC = 0x40,
    DEC = 0x48,

    CMP = 0x3B,
    CMP_R = 0x39,
    JMP = 0xE9,
    JG = 0x8F,
    JGE = 0x8D,
    JL = 0x8C,
    JLE = 0x8E,
    JE = 0x75,
    JNE = 0x85,
};

enum Registers
{
    RAX,
    RBX,
    RCX,
    RDX,
};

#endif //BINARY_TRANSLATOR_TRANSLATOR_H