#include <iostream>
#include <fstream>
#include <vector>

    

class Instruction {
private:    
    int m_Id;
    int m_argType;
    
    int m_arg1;
    int m_arg2;

    std::string m_label;

public: 
    Instruction(const int id, const int argType,
                const int arg1 = 0, const int arg2 = 0) : 
                    m_Id(id), m_argType(argType), m_arg1(arg1), m_arg2(arg2) {}

    Instruction(const int id, const int argType, const std::string label) : 
                    m_Id(id), m_argType(argType), m_label(label) {}

    void Dump()
    {
        std::cout << "Id [" << m_Id << "], ArgType [" << m_argType << 
        "]\n\tArg_1 [" << m_arg1 << "] Arg_2 [" << m_arg2 << "]\n\tlabel: {" << m_label << "}\n\n";
    }                    
};

enum REGISTERS {
    RAX,
    RBX,
    RCX,
    RDX,
};

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
    CMP_R,
    JMP,
    JA,
    JEA,
    JB,
    JBE,
    JE,
    JNE
};

enum ARGTYPES {
    NOARG,
    LABEL,
    NUMBER,
    REG,
    REG_REG,
    REG_NUMBER,
};

void ReadFromFile(char *pathToFile, std::vector<std::string> &instructionsText);

void ParseInstructions(const std::vector<std::string> &instructionsText,
                             std::vector<Instruction> &instructions);

int WhichReg(const std::string &instructionText, const bool isFirstArg = true);

Instruction ParseArguments(const int id, const int argType, std::string instructionText);