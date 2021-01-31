#ifndef CPU_SIMULATOR_INSTRUCTION_H
#define CPU_SIMULATOR_INSTRUCTION_H

#include <experimental/propagate_const>
#include <map>
#include <memory>
#include <deque>

namespace CPU_Simulator
{

    struct OffsetLabel
    {
        std::deque<size_t> froms;
        size_t to;
    };

    class Instruction
    {
    private:
        class Impl;
        std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl_;

    public:
        Instruction();
        
        Instruction(const Instruction&) = delete;
        Instruction& operator=(const Instruction&) = delete;
        Instruction(Instruction&&);

        ~Instruction();

        void ParseInstruction(const std::string &instructionText);
        std::string ConvertToByteCode
            (std::map<std::string, OffsetLabel> &labels, int offset) const;

        int GetArgType() const;
        std::string GetLabel() const;
        std::string GetLabeled() const;

        void SetLabeled(const std::string &labeled);

        void Dump() const;

    }; //class Instruction

    enum REGISTERS
    {
        RAX,
        RBX,
        RCX,
        RDX,
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

    enum ARGTYPES
    {
        NOARG,
        LABEL,
        NUMBER,
        REG,
        REG_REG,
        REG_NUMBER,
    };

} //namespace CPU_Simulator

#endif //CPU_SIMULATOR_INSTRUCTION_H