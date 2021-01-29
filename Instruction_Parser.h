#include <iostream>

#include <vector>
#include <map>

#pragma once


class Instruction
{
private:
    int Id_ = -1;
    int argType_ = -1;

    int arg1_ = 0;
    int arg2_ = 0;
    std::string label_;

    void ParseArguments(const std::string& instructionText);    

public :
    Instruction(const int id, const int argType,
                const int arg1 = 0, const int arg2 = 0) :
                 Id_(id), argType_(argType), arg1_(arg1), arg2_(arg2)
    {
    }

    Instruction(const int id, const int argType, const std::string label) : 
                Id_(id), argType_(argType), label_(label) {}

    void Dump() const;

    void ParseInstruction(const std::string &instructionText);
};

enum REGISTERS
{
    RAX,
    RBX,
    RCX,
    RDX,
};

enum INSTRUCTIONS
{
    PUSH,
    PUSH_R,
    POP_R,
    MOV,
    MOV_R,
    CALL,
    RET,
    EXIT,
    WRITE,
    READ,

    ADD_R,
    SUB_R,
    MUL_R,
    DIV_R,
    ADD,
    SUB,
    MUL,
    DIV,
    INC,
    DEC,

    CMP,
    CMP_R,
    JMP,
    JA,
    JEA,
    JB,
    JBE,
    JE,
    JNE
};

enum ARGTYPES
{
    NOARG,
    LABEL,
    NUMBER,
    REG,
    REG_REG,
    REG_NUMBER,
};