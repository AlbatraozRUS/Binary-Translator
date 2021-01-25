#include <iostream>
#include <fstream>
#include <vector>

class Instruction {
private:
    std::string m_name;
    int m_ID;
    
    int m_arg1;
    int m_arg2;

public: 
    Instruction(const std::string& name, const int ID, 
                const int arg1 = -1, const int arg2 = -1) : 
                    m_name(name), m_ID(ID), m_arg1(arg1), m_arg2(arg2) {}
                    
};

enum REGISTERS {
    RAX,
    RBX,
    RCX,
    RDX,
}

enum INSTRUCTIONS {
    PUSH,
    PUSH_R,
    POP_R,
    MOV,
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
    JMP,
    JA,
    JEA,
    JB,
    JBE,
    JE,
    JNE
}

void ReadFromFile(char *pathToFile, std::vector<std::string> &instructions);