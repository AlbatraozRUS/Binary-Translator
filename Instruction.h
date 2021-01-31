#ifndef CPU_SIMULATOR_INSTRUCTION_H
#define CPU_SIMULATOR_INSTRUCTION_H

#include <experimental/propagate_const>
#include <map>
#include <memory>

namespace CPU_Simulator
{

    struct OffsetLabel
    {
        size_t from;
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
    //    Instruction& operator=(Instruction&&);

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

} //namespace CPU_Simulator

#endif //CPU_SIMULATOR_INSTRUCTION_H