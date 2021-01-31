#include <iostream>

#include <vector>
#include <map>

#pragma once

struct OffsetLabel
{
    size_t from;
    size_t to;
};

class Instruction
{
private:
    int Id_ = -1;
    int argType_ = -1;

    int arg1_ = 0;
    int arg2_ = 0;
    std::string label_;
    std::string labeled_;

    void ParseArguments(const std::string& instructionText);    

public:

    Instruction()  = default;

    ~Instruction() = default;

    void Dump() const;

    void ParseInstruction(const std::string &instructionText);

    std::string Convert2ByteCode
            (std::map<std::string, OffsetLabel> &labels, const int offset) const;

    int GetArgType() const;

    std::string GetLabel() const;

    std::string GetLabeled() const;

    void SetLabeled(const std::string& labeled);
};

enum REGISTERS {
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
    JAE,
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

const std::map<int, std::string> kRegisterList = {{RAX, "rax"}, 
                                                  {RBX, "rbx"}, 
                                                  {RCX, "rcx"}, 
                                                  {RDX, "rdx"}};