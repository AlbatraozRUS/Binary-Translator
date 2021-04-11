#ifndef BINARY_TRANSLATOR_COMMON_CONSTANTS_H_
#define BINARY_TRANSLATOR_COMMON_CONSTANTS_H_

namespace BinaryTranslator {

enum NumInstructions {
    NUM_PUSH = 0,
    NUM_PUSH_R,
    NUM_POP_R,
    NUM_MOV,
    NUM_MOV_R,
    NUM_MOV_RP,
    NUM_MOV_PR,
    NUM_CALL,
    NUM_RET,
    NUM_EXIT,
    NUM_WRITE,
    NUM_READ,

    NUM_ADD,
    NUM_SUB,
    NUM_IMUL,
    NUM_IDIV,
    NUM_ADD_R,
    NUM_SUB_R,
    NUM_IMUL_R,
    NUM_IDIV_R,
    NUM_INC,
    NUM_DEC,

    NUM_CMP,
    NUM_CMP_R,
    NUM_JMP,
    NUM_JG,
    NUM_JGE,
    NUM_JL,
    NUM_JLE,
    NUM_JE,
    NUM_JNE,
    N_INST,
};

enum IdInstructions {
    PUSH = 0x68,
    PUSH_R = 0x50,
    POP_R = 0x58,
    MOV = 0xB8,
    MOV_R = 0xBB,
    MOV_RP = 0xBC,
    MOV_PR = 0xBD,
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

enum Registers {
    RAX,
    RBX,
    RCX,
    RDX,
    N_REGS,
};

enum Argtypes {
    NOARG,
    LABEL,
    NUMBER,
    REG,
    REG_REG,
    REG_NUMBER,
};

} //namespace BinaryTranslator

#endif // BINARY_TRANSLATOR_COMMON_CONSTANTS_H_