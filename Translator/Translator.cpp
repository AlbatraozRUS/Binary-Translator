
#include "Translator.h"

#include "Constants.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"

#include <iostream>
#include <random>
#include <stack>

using namespace BinaryTranslator;

namespace {

const int MAX_RANDOM = 100;
const int MIN_RANDOM = 0;

int GetNumberIdInstr(int idInstr)
{
    #define INSTRUCTION(name, id, argType, num, size, code)  \
        case id: return num;                                 \

    #define INSTRUCTIONS
    switch (idInstr) {
    #include "Commands_DSL.txt"

    default:
        throw std::runtime_error("GetNumberIdInstr():"
                                 "Unidefined instruction " +
                                 std::to_string(idInstr));
    }

    #undef INSTRUCTIONS
    #undef INSTRUCTION
}

int GetArgtypeInstr(int idInstr)
{
    #define INSTRUCTION(name, id, argType, num, size, code)  \
        case id: return argType;                             \

    #define INSTRUCTIONS
    switch (idInstr) {
    #include "Commands_DSL.txt"

    case N_INST: return N_INST;
    default:
        throw std::runtime_error("GetArgtypeInstr():"
                                 "Unidefined instruction " +
                                 std::to_string(idInstr));
    }

    #undef INSTRUCTIONS
    #undef INSTRUCTION
}

bool IsRegRegInst(int inst)
{
    if (GetArgtypeInstr(inst) == REG_REG)
        return true;
    return false;
}

bool IsJumpInstr(int inst)
{
    if (GetArgtypeInstr(inst) == LABEL)
        return true;
    return false;
}

int GetRandomNumber(int min, int max)
{
    std::uniform_int_distribution<> UID{min, max};
    static std::default_random_engine RE{};

    return UID(RE);
}

} // anonymous namespace


class Translator::Impl {
private:
    const size_t SIZE_MEMORY = 1000;
    const size_t SIZE_MEMORY_BENCHMARK = SIZE_MEMORY;

    std::string pathToInputFile_;

    unsigned char* bytecode_ = nullptr;
    size_t sizeByteCode_ = 0;
    size_t PC_ = 0;
    std::string output_;

    llvm::LLVMContext context_;
    llvm::Module* module_       = nullptr;
    llvm::Function* curFunc_    = nullptr;
    llvm::IRBuilder<>* builder_ = nullptr;

    std::stack<llvm::Value*> stackIR_;
    llvm::Value* curCmpValue_   = nullptr;

    struct GlobalArray {
        size_t size = 0;
        std::string name{};
        llvm::ArrayType* type       = nullptr;
        llvm::GlobalVariable* array = nullptr;
    };

    GlobalArray regs_ {
        .size = N_REGS,
        .name = "regs",
    };

    GlobalArray memory_ {
        .size = SIZE_MEMORY,
        .name = "memory",
    };

    GlobalArray benchmarkResult_ {
        .size = N_INST + 1,
        .name = "nTacts",
    };

    struct BranchBB {
        llvm::BasicBlock* trueBB  = nullptr;
        size_t truePC = 0;

        llvm::BasicBlock* falseBB = nullptr;
        size_t falsePC = 0;
    };

    struct TranslatedValue {
        llvm::Value* ptr = nullptr;
        llvm::Value* val = nullptr;
    };

    std::map <size_t, BranchBB> branchBBs_;
    std::map <size_t, llvm::Function*> functions_;

    bool isAnalyse_ = false;

    llvm::Function* CreateFunc();
    BranchBB CreateBranchBB(size_t truePC, size_t falsePC);

    void ReadBytecode();
    void CreateGlobalArray(GlobalArray& GA);

    void TranslateByteCode();
    void TranslateByteCodeExpression();
    void TranslateByteCodeJumps();
    void TranslateByteCodeCmp();
    void TranslateByteCodeIO();
    void TranslateByteCodeStack();
    void TranslateByteCodeCall();
    void TranslateByteCodeRet();
    void TranslateByteCodeExit();

    TranslatedValue TranslateRegister(size_t PC);
    llvm::Value* TranslateMemory(llvm::Value* val);

    llvm::Function*   GetFunction(size_t PC) const;
    llvm::BasicBlock* GetBB      (size_t PC) const;
    void MovePC();

    void CountTact(int idInst);
    void PrintBenchmarkResult();

public:
    Impl(char*  pathToInputFile, bool isAnalyse = false) :
        pathToInputFile_(pathToInputFile),
        isAnalyse_(isAnalyse)
        {}

    ~Impl()
    {
        delete[] bytecode_;
    }

    void Translate();
    void PreTranslateBenchmark();
    void PreTranslate();

    friend void Translator::Dump() const;

}; // class Translator::Impl


void Translator::Impl::PreTranslate()
{
    ReadBytecode();

    // Create basic
    module_  = new llvm::Module("top", context_);
    builder_ = new llvm::IRBuilder(context_);

    llvm::FunctionType* funcType =
            llvm::FunctionType::get(builder_->getInt32Ty(), false);
    llvm::Function* mainFunc =
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                   "main", module_);

    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(context_, "entryBB",
                                                         mainFunc);

    builder_->SetInsertPoint(entryBB);
    curFunc_ = mainFunc;

    for (; PC_ < sizeByteCode_; MovePC()) {
        if (bytecode_[PC_] == CALL)
            if (GetFunction(PC_ + (char)bytecode_[PC_ + 1]) == nullptr)
                functions_.insert(std::make_pair
                    (PC_ + (char)bytecode_[PC_ + 1], CreateFunc()));

        if (IsJumpInstr(bytecode_[PC_]))
            branchBBs_.insert(std::make_pair
                (PC_, CreateBranchBB(PC_ + (char)bytecode_[PC_ + 1], PC_ + 2)));
    }

    curFunc_ = mainFunc;
    PC_ = 0;

    CreateGlobalArray(memory_);
}

void Translator::Impl::PreTranslateBenchmark()
{
    if (!isAnalyse_)
        return;

    for (size_t i = 0; i < SIZE_MEMORY_BENCHMARK; i++) {
        // int number = GetRandomNumber(MIN_RANDOM, MAX_RANDOM);
        int number = memory_.size - i;
        llvm::Value* pArg_1 = builder_->CreateConstGEP2_32(memory_.type,
                                                           memory_.array, 0, i);
        llvm::Value* arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(),
                                                  number);
        builder_->CreateStore(arg_2, pArg_1);
    }

    llvm::Value* startArrayBenchmark =
        llvm::ConstantInt::get(builder_->getInt32Ty(), 0);
    llvm::Value* endArrayBenchmark =
        llvm::ConstantInt::get(builder_->getInt32Ty(), SIZE_MEMORY_BENCHMARK);

    stackIR_.push(startArrayBenchmark);
    stackIR_.push(endArrayBenchmark);
}

void Translator::Impl::Translate()
{
    CreateGlobalArray(regs_);
    CreateGlobalArray(benchmarkResult_);
    TranslateByteCode();
}

void Translator::Impl::TranslateByteCode()
{
    llvm::Function* tmpFunc = nullptr;
    llvm::BasicBlock* tmpBB = nullptr;
    while (PC_ < sizeByteCode_) {

        tmpFunc = GetFunction(PC_);
        if (tmpFunc != nullptr)
            curFunc_ = tmpFunc;

        tmpBB = GetBB(PC_);
        if (tmpBB != nullptr)
            builder_->SetInsertPoint(tmpBB);

        CountTact(bytecode_[PC_]);

        switch (bytecode_[PC_]) {
        case ADD_R:
        case ADD:
        case SUB_R:
        case SUB:
        case IMUL_R:
        case IMUL:
        case IDIV_R:
        case IDIV:
        case INC:
        case DEC:
        case MOV:
        case MOV_R:
        case MOV_RP:
        case MOV_PR:
        case MOV_PP:
            TranslateByteCodeExpression();
            break;

        case JMP:
        case JG:
        case JGE:
        case JL:
        case JLE:
        case JE:
        case JNE:
            TranslateByteCodeJumps();
            break;

        case CMP:
        case CMP_R:
        case CMP_RP:
        case CMP_PP:
            TranslateByteCodeCmp();
            break;

        case WRITE:
        case WRITE_P:
        case READ:
        case READ_P:
            TranslateByteCodeIO();
            break;

        case PUSH:
        case PUSH_R:
        case POP_R:
            TranslateByteCodeStack();
            break;

        case CALL:
            TranslateByteCodeCall();
            break;

        case RET:
            TranslateByteCodeRet();
            break;

        case EXIT:
            TranslateByteCodeExit();
            break;

        default:
            throw std::runtime_error("TranslateByteCode():"
                                     "Unidefined instruction" +
                                     std::to_string(bytecode_[PC_]));
        }
    }
}

llvm::Function* Translator::Impl::CreateFunc()
{
    static size_t numFunc = 0;
    numFunc++;

    llvm::FunctionType* funcType =
        llvm::FunctionType::get(builder_->getVoidTy(), false);
    llvm::Function* function =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                "Function" + std::to_string(numFunc), module_);

    curFunc_ = function;
    size_t startFuncPC = PC_ + (char)bytecode_[PC_ + 1];
    branchBBs_.insert(std::make_pair(startFuncPC, CreateBranchBB(startFuncPC,
                                                                 startFuncPC)));

    return function;
}

Translator::Impl::BranchBB Translator::Impl::CreateBranchBB(size_t truePC,
                                                            size_t falsePC)
{
    static size_t numBB = 0;
    numBB++;

    BranchBB branchBB;

    llvm::BasicBlock* tmpTrueBB = GetBB(truePC);
    if (tmpTrueBB == nullptr)
        branchBB.trueBB =
            llvm::BasicBlock::Create(context_,
                                     "trueBB" + std::to_string(numBB),
                                     curFunc_);
    else
        branchBB.trueBB = tmpTrueBB;
    branchBB.truePC = truePC;

    if (truePC == falsePC || bytecode_[PC_] == JMP)
        return branchBB;

    llvm::BasicBlock* tmpFalseBB = GetBB(falsePC);
    if (tmpFalseBB == nullptr)
        branchBB.falseBB =
            llvm::BasicBlock::Create(context_,
                                     "falseBB" + std::to_string(numBB),
                                     curFunc_);
    else
        branchBB.falseBB = tmpFalseBB;
    branchBB.falsePC = falsePC;

    return branchBB;
}

void Translator::Impl::TranslateByteCodeExpression()
{
    TranslatedValue arg_1 = TranslateRegister(PC_ + 1);

    TranslatedValue arg_2{};
    if (IsRegRegInst(bytecode_[PC_]))
        arg_2 = TranslateRegister(PC_ + 2);
    else
        arg_2.val = llvm::ConstantInt::get(builder_->getInt32Ty(),
                                            bytecode_[PC_ + 2]);

    llvm::Value* res = nullptr;
    switch (bytecode_[PC_]) {
    case MOV_PP:
        arg_1.ptr = TranslateMemory(arg_1.val);
        arg_2.ptr = TranslateMemory(arg_2.val);
        res = builder_->CreateLoad(arg_2.ptr);
        break;

    case MOV_PR:
        arg_1.ptr = TranslateMemory(arg_1.val);
        res = arg_2.val;
        break;

    case MOV_RP:
        arg_2.ptr = TranslateMemory(arg_2.val);
        res = builder_->CreateLoad(arg_2.ptr);
        break;

    case ADD:
    case ADD_R:
        res = builder_->CreateAdd(arg_1.val, arg_2.val);
        break;

    case SUB:
    case SUB_R:
        res = builder_->CreateSub(arg_1.val, arg_2.val);
        break;

    case IMUL:
    case IMUL_R:
        res = builder_->CreateMul(arg_1.val, arg_2.val);
        break;

    case IDIV:
    case IDIV_R:
        res = builder_->CreateSDiv(arg_1.val, arg_2.val);
        break;

    case INC:
        arg_2.val = llvm::ConstantInt::get(builder_->getInt32Ty(), 1);
        res = builder_->CreateAdd(arg_1.val, arg_2.val);
        break;

    case DEC:
        arg_2.val = llvm::ConstantInt::get(builder_->getInt32Ty(), 1);
        res = builder_->CreateSub(arg_1.val, arg_2.val);
        break;

    case MOV:
    case MOV_R:
        res = arg_2.val;
        break;

    default:
        throw std::runtime_error("TranslateByteCodeExpression():"
                                 "Unidefined instruction" +
                                 std::to_string(bytecode_[PC_]));
    }

    builder_->CreateStore(res, arg_1.ptr);

    MovePC();
}

void Translator::Impl::TranslateByteCodeJumps()
{
    llvm::BasicBlock* trueBB = GetBB(PC_ + (char)bytecode_[PC_ + 1]);
    llvm::BasicBlock* falseBB = GetBB(PC_ + 2);
    if (bytecode_[PC_] == JMP)
        builder_->CreateBr(trueBB);
    else
        builder_->CreateCondBr(curCmpValue_, trueBB, falseBB);

    MovePC();
}

void Translator::Impl::TranslateByteCodeCmp()
{
    TranslatedValue arg_1 = TranslateRegister(PC_ + 1);

    TranslatedValue arg_2{};
    if (IsRegRegInst(bytecode_[PC_]))
        arg_2 = TranslateRegister(PC_ + 2);
    else
        arg_2.val = llvm::ConstantInt::get(builder_->getInt32Ty(),
                                       bytecode_[PC_ + 2]);

    switch (bytecode_[PC_]) {
        case CMP_PP:
            arg_1.ptr = TranslateMemory(arg_1.val);
            arg_1.val = builder_->CreateLoad(arg_1.ptr);
        case CMP_RP:
            arg_2.ptr = TranslateMemory(arg_2.val);
            arg_2.val = builder_->CreateLoad(arg_2.ptr);
            break;
    }

    llvm::CmpInst::Predicate predicate;
    switch (bytecode_[PC_ + 3]) {
        case JMP: return;
        case JG:  predicate = llvm::CmpInst::Predicate::ICMP_SGT;  break;
        case JGE: predicate = llvm::CmpInst::Predicate::ICMP_SGE;  break;
        case JL:  predicate = llvm::CmpInst::Predicate::ICMP_SLT;  break;
        case JLE: predicate = llvm::CmpInst::Predicate::ICMP_SLE;  break;
        case JE:  predicate = llvm::CmpInst::Predicate::ICMP_EQ;   break;
        case JNE: predicate = llvm::CmpInst::Predicate::ICMP_NE;   break;
        default:
            throw std::runtime_error("TranslateByteCodeCmp():"
                                     "Jump instruction is not found");
    }

    curCmpValue_ = builder_->CreateCmp(predicate, arg_1.val, arg_2.val,
                                       "resCmp");
    MovePC();
}

void Translator::Impl::TranslateByteCodeIO()
{
    std::vector<llvm::Type *> funcArgsTypes;
    funcArgsTypes.push_back(builder_->getInt8PtrTy());
    llvm::FunctionType* funcType =
                                 llvm::FunctionType::get(builder_->getInt32Ty(),
                                                         funcArgsTypes, true);

    std::string formatStr("%d");
    llvm::FunctionCallee func;
    llvm::FunctionCallee printfReg = module_->getOrInsertFunction("printf",
                                                                  funcType);
    TranslatedValue arg = TranslateRegister(PC_ + 1);
    switch (bytecode_[PC_]) {
    case WRITE_P:
        arg.ptr = TranslateMemory(arg.val);
    case WRITE:
        arg.val = builder_->CreateLoad(arg.ptr);
        func = printfReg;
        formatStr += "\n";
        break;

    case READ_P:
        arg.ptr = TranslateMemory(arg.val);
    case READ:
        arg.val = arg.ptr;
        func = module_->getOrInsertFunction("scanf", funcType);
        break;

    default:
        throw std::runtime_error("TranslateByteCodeIO():"
                                 "Unidefined instruction"
                                 + std::to_string(bytecode_[PC_]));
    }

    std::string regStr;
    switch (bytecode_[PC_ + 1]) {
    case EAX:
        regStr += "EAX: = ";
        break;
    case EBX:
        regStr += "EBX: = ";
        break;
    case ECX:
        regStr += "ECX: = ";
        break;
    case EDX:
        regStr += "RDX: = ";
        break;

    default:
        throw std::runtime_error("TranslateByteCodeIO():"
                                 "Undefined instruction" +
                                 std::to_string(bytecode_[PC_ + 1]));
    }
    llvm::Value* formatRegStrVal =
        builder_->CreateGlobalStringPtr(regStr, "IORegister");
    llvm::Value* formatStrVal =
        builder_->CreateGlobalStringPtr(formatStr, "IOformat");

    std::vector<llvm::Value *> args;
    args.push_back(formatRegStrVal);
    builder_->CreateCall(printfReg, args);

    args.clear();

    args.push_back(formatStrVal);
    args.push_back(arg.val);

    builder_->CreateCall(func, args);

    MovePC();
}

void Translator::Impl::TranslateByteCodeStack()
{
    llvm::Value* pArg = nullptr;
    llvm::Value* arg = nullptr;

    switch (bytecode_[PC_]) {
    case PUSH:
        arg = llvm::ConstantInt::get(builder_->getInt32Ty(),
                                     (char)bytecode_[PC_ + 1]);
        stackIR_.push(arg);
        break;

    case PUSH_R:
        pArg = builder_->CreateConstGEP2_32(regs_.type, regs_.array, 0,
                                            bytecode_[PC_ + 1]);
        arg = builder_->CreateLoad(pArg);
        stackIR_.push(arg);
        break;

    case POP_R:
        pArg = builder_->CreateConstGEP2_32(regs_.type, regs_.array, 0,
                                            bytecode_[PC_ + 1]);
        arg = stackIR_.top();
        builder_->CreateStore(arg, pArg);
        stackIR_.pop();
        break;

    default:
        throw std::runtime_error("TranslateByteCodeStack():"
                                 "Unidefined instruction"
                                 + std::to_string(bytecode_[PC_]));
    }

    MovePC();
}

void Translator::Impl::TranslateByteCodeCall()
{
    llvm::Function* function = GetFunction(PC_ + (char)bytecode_[PC_ + 1]);
    builder_->CreateCall(function);

    MovePC();
}

void Translator::Impl::TranslateByteCodeRet()
{
    builder_->CreateRetVoid();
    MovePC();
}

void Translator::Impl::TranslateByteCodeExit()
{
    PrintBenchmarkResult();
    builder_->CreateRet(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));
    MovePC();
}

Translator::Impl::TranslatedValue Translator::Impl::TranslateRegister(size_t PC)
{
    TranslatedValue arg{};
    arg.ptr    = builder_->CreateConstGEP2_32(regs_.type, regs_.array,
                                              0, bytecode_[PC]);
    arg.val  = builder_->CreateLoad(arg.ptr);

    return arg;
}

llvm::Value* Translator::Impl::TranslateMemory(llvm::Value* val)
{
    llvm::Value* tmp = llvm::ConstantInt::get(builder_->getInt32Ty(), 0);
    llvm::ArrayRef<llvm::Value*> idxList = {tmp, val};
    return builder_->CreateGEP(memory_.type, memory_.array, idxList);
}

// End of functions


void Translator::Impl::ReadBytecode()
{
    FILE *inputFile = fopen(pathToInputFile_.c_str(), "rb");

    if (!inputFile)
        throw std::runtime_error("Simulator: Can`t open input file");

    fseek(inputFile, 0, SEEK_END);
    sizeByteCode_ = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    bytecode_ = new unsigned char[sizeByteCode_];

    fread(bytecode_, 1, sizeByteCode_, inputFile);

    fclose(inputFile);
}

void Translator::Impl::CreateGlobalArray(GlobalArray& GA)
{
    GA.type = llvm::ArrayType::get(builder_->getInt32Ty(), GA.size);
    module_->getOrInsertGlobal(GA.name, GA.type);
    GA.array = module_->getNamedGlobal(GA.name);

    std::vector<llvm::Constant *> temp;
    for (size_t i = 0; i < GA.size; i++)
        temp.push_back(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));

    GA.array->setInitializer(llvm::ConstantArray::get(GA.type, temp));
}

void Translator::Impl::MovePC()
{
    #define INSTRUCTION(name, id, argType, num, size, code)  \
        case id: PC_ += size; break;                         \

    #define INSTRUCTIONS
    switch (bytecode_[PC_]) {
    #include "Commands_DSL.txt"

    default:
        throw std::runtime_error("TranslateByteCode():Unidefined instruction" +
                                 std::to_string(bytecode_[PC_]));
    }

    #undef INSTRUCTIONS
    #undef INSTRUCTION
}

void Translator::Impl::CountTact(int idInst)
{
    if (!isAnalyse_)
        return;

    llvm::Value* arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(), 1);

    llvm::Value* pArg_1 =
            builder_->CreateConstGEP2_32(benchmarkResult_.type,
                                         benchmarkResult_.array,
                                         0, GetNumberIdInstr(idInst));
    llvm::Value* arg_1 = builder_->CreateLoad(pArg_1);
    builder_->CreateStore(builder_->CreateAdd(arg_1, arg_2), pArg_1);

    pArg_1 =
            builder_->CreateConstGEP2_32(benchmarkResult_.type,
                                         benchmarkResult_.array,
                                         0, N_INST);
    arg_1 = builder_->CreateLoad(pArg_1);
    builder_->CreateStore(builder_->CreateAdd(arg_1, arg_2), pArg_1);
}

void Translator::Impl::PrintBenchmarkResult()
{
    if (!isAnalyse_)
        return;

    std::vector<llvm::Type *> funcArgsTypes;
    funcArgsTypes.push_back(builder_->getInt8PtrTy());
    llvm::FunctionType *funcType =
        llvm::FunctionType::get(builder_->getInt32Ty(),
                                funcArgsTypes, true);

    llvm::FunctionCallee func = module_->getOrInsertFunction("printf",
                                                             funcType);

    std::string formatStr{"\n\n[Result of Benchmark]\nVer 1.0.0\n"
                                "Number of tacts of each instruction:\n"};
    #define INSTRUCTION(name, id, argType, num, size, code)  \
        formatStr += "\t";                                   \
        formatStr += #name;                                  \
        formatStr += " - %d\n";                              \

    #define INSTRUCTIONS
    #include "Commands_DSL.txt"

    #undef INSTRUCTIONS
    #undef INSTRUCTION
    formatStr += "\n\tTotal amount of instructions - %d\n"
                 "\n[End!]\n\n";

    llvm::Value *formatStrVal =
        builder_->CreateGlobalStringPtr(formatStr, "BenchmarkResult");

    std::vector<llvm::Value*> args;
    args.push_back(formatStrVal);
    for (size_t iNumInst = 0; iNumInst < N_INST + 1; iNumInst++) {
        llvm::Value* pArg = builder_->CreateConstGEP2_32(benchmarkResult_.type,
                                                         benchmarkResult_.array,
                                                         0, iNumInst);
        llvm::Value* arg = builder_->CreateLoad(pArg);
        args.push_back(arg);
    }

    builder_->CreateCall(func, args);
}

llvm::BasicBlock* Translator::Impl::GetBB(size_t PC) const
{
    for (auto branchBB : branchBBs_) {
        if (branchBB.second.truePC == PC)
            return branchBB.second.trueBB;
        if (branchBB.second.falsePC == PC)
            return branchBB.second.falseBB;
    }

    return nullptr;
}

llvm::Function* Translator::Impl::GetFunction(size_t PC) const
{
    auto function = functions_.find(PC);
    if (function != functions_.end())
        return function->second;

    return nullptr;
}

// End of functions of class Translator::Impl ----------------------------------

Translator::Translator(char* pathToInputFile, bool isAnalyse) :
    pImpl_(std::make_unique<Impl>(pathToInputFile, isAnalyse)) {};

Translator::~Translator() = default;

Translator::Translator(Translator &&) = default;

void Translator::Translate()
{
    pImpl_->PreTranslate();
    pImpl_->PreTranslateBenchmark();
    pImpl_->Translate();
}

void Translator::Dump() const
{
    std::cout << ";#[LLVM_IR]:\n";
    std::string s;
    llvm::raw_string_ostream os(s);
    pImpl_->module_->print(os, nullptr);
    os.flush();
    std::cout << s;
}
