
#include "Translator.h"

#include <iostream>
#include <stack>

#include "llvm-11/llvm/IR/BasicBlock.h"
#include "llvm-11/llvm/IR/Function.h"
#include "llvm-11/llvm/IR/IRBuilder.h"
#include "llvm-11/llvm/IR/LLVMContext.h"
#include "llvm-11/llvm/IR/Value.h"

namespace {

bool IsRegRegInst(const int inst)
{
    if (inst == ADD_R  || inst == SUB_R  ||
        inst == IMUL_R || inst == IDIV_R ||
        inst == CMP_R  || inst == MOV_R)
        return true;

    return false;
}

bool IsJumpInst(const int inst)
{
    if (inst == JMP || inst == JG || inst == JGE || inst == JL ||
        inst == JLE || inst == JE || inst == JNE)
        return true;
    return false;
}
} // namespace

class Translator::Impl {
private:
    std::string pathToInputFile_;

    unsigned char* bytecode_ = nullptr;
    size_t sizeByteCode_ = 0;
    size_t PC_ = 0;
    std::string output_;

    llvm::LLVMContext context_;
    llvm::Module* module_       = nullptr;
    llvm::Function* curFunc_    = nullptr;
    llvm::IRBuilder<>* builder_ = nullptr;
    llvm::GlobalVariable* regs_ = nullptr;
    llvm::ArrayType* regsType_  = nullptr;
    llvm::Value* curCmpValue_   = nullptr;

    std::stack<llvm::Value *> stackIR_;

    struct BranchBB {
        llvm::BasicBlock* trueBB  = nullptr;
        size_t truePC = 0;

        llvm::BasicBlock* falseBB = nullptr;
        size_t falsePC = 0;
    };

    std::map <size_t, BranchBB> branchBBs_;
    std::map <size_t, llvm::Function*> functions_;

    llvm::Function* CreateFunc();
    BranchBB CreateBranchBB(size_t truePC, size_t falsePC);

    void TranslateByteCode();
    void ReadBytecode();
    void AllocByteCodeBuf();

    void TranslateByteCodeExpression();
    void TranslateByteCodeJumps();
    void TranslateByteCodeCmp();
    void TranslateByteCodeIO();
    void TranslateByteCodeStack();
    void TranslateByteCodeCall();
    void TranslateByteCodeRet();
    void TranslateByteCodeExit();

    llvm::Function *GetFunction(size_t PC) const;
    llvm::BasicBlock* GetBB(size_t PC) const;
    void MovePC();

public:
    Impl(char *const pathToInputFile) : pathToInputFile_(pathToInputFile) {}

    ~Impl()
    {
        delete[] bytecode_;
    }

    void Translate();
    void Preprocess();
    void PreprocessBenchmark();

    friend void Translator::Dump() const;

}; // class Translator::Impl

void Translator::Impl::Preprocess()
{
    ReadBytecode();

    // Create basic
    module_  = new llvm::Module("top", context_);
    builder_ = new llvm::IRBuilder(context_);

    llvm::FunctionType *funcType =
            llvm::FunctionType::get(builder_->getInt32Ty(), false);
    llvm::Function *mainFunc =
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                   "main", module_);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context_, "entryBB",
                                                         mainFunc);

    builder_->SetInsertPoint(entryBB);
    curFunc_ = mainFunc;

    for (; PC_ < sizeByteCode_; MovePC()) {
        if (bytecode_[PC_] == CALL)
            if (GetFunction(PC_ + (char)bytecode_[PC_ + 1]) == nullptr)
                functions_.insert(std::make_pair
                    (PC_ + (char)bytecode_[PC_ + 1], CreateFunc()));

        if (IsJumpInst(bytecode_[PC_]))
            branchBBs_.insert(std::make_pair
                (PC_, CreateBranchBB(PC_ + (char)bytecode_[PC_ + 1], PC_ + 2)));
    }

    curFunc_ = mainFunc;
    PC_ = 0;
}

void Translator::Impl::Translate()
{
    // Create global array of registers
    regsType_ = llvm::ArrayType::get(builder_->getInt32Ty(), NREGS);
    module_->getOrInsertGlobal("regs_", regsType_);
    regs_ = module_->getNamedGlobal("regs_");

    std::vector<llvm::Constant *> temp;
    for (size_t i = 0; i < NREGS; i++)
        temp.push_back(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));

    regs_->setInitializer(llvm::ConstantArray::get(regsType_, temp));

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
            TranslateByteCodeCmp();
            break;

        case WRITE:
        case READ:
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

    llvm::FunctionType *funcType =
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
            llvm::BasicBlock::Create(context_, "trueBB" + std::to_string(numBB),
                                     curFunc_);
    else
        branchBB.trueBB = tmpTrueBB;
    branchBB.truePC = truePC;

    if (truePC == falsePC || bytecode_[PC_] == JMP)
        return branchBB;

    llvm::BasicBlock *tmpFalseBB = GetBB(falsePC);
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

    llvm::Value *pArg_1 = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                                       bytecode_[PC_ + 1]);
    llvm::Value *arg_1 = builder_->CreateLoad(pArg_1);

    llvm::Value *arg_2 = nullptr;
    if (IsRegRegInst(bytecode_[PC_])) {
        llvm::Value *pArg_2 = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                                           bytecode_[PC_ + 2]);
        arg_2 = builder_->CreateLoad(pArg_2);
    }
    else
        arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(),
                                       (char)bytecode_[PC_ + 2]);

    llvm::Value *res = nullptr;
    switch (bytecode_[PC_]) {
    case ADD:
    case ADD_R:
        res = builder_->CreateAdd(arg_1, arg_2);
        break;

    case SUB:
    case SUB_R:
        res = builder_->CreateSub(arg_1, arg_2);
        break;

    case IMUL:
    case IMUL_R:
        res = builder_->CreateMul(arg_1, arg_2);
        break;

    case IDIV:
    case IDIV_R:
        res = builder_->CreateSDiv(arg_1, arg_2);
        break;

    case INC:
        arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(), 1);
        res = builder_->CreateAdd(arg_1, arg_2);
        break;

    case DEC:
        arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(), 1);
        res = builder_->CreateSub(arg_1, arg_2);
        break;

    case MOV:
    case MOV_R:
        res = arg_2;
        break;

    default:
        throw std::runtime_error("TranslateByteCodeExpression():"
                                 "Unidefined instruction" +
                                 std::to_string(bytecode_[PC_]));
    }

    builder_->CreateStore(res, pArg_1);

    MovePC();
}

void Translator::Impl::TranslateByteCodeJumps()
{
    llvm::BasicBlock* trueBB = GetBB(PC_ + (char)bytecode_[PC_ + 1]);
    llvm::BasicBlock *falseBB = GetBB(PC_ + 2);
    if (bytecode_[PC_] == JMP)
        builder_->CreateBr(trueBB);
    else
        builder_->CreateCondBr(curCmpValue_, trueBB, falseBB);

    MovePC();
}

void Translator::Impl::TranslateByteCodeCmp()
{
    llvm::Value *pArg_1 = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                                       bytecode_[PC_ + 1]);
    llvm::Value *arg_1 = builder_->CreateLoad(pArg_1);

    llvm::Value *arg_2 = nullptr;
    if (IsRegRegInst(bytecode_[PC_]))
    {
        llvm::Value *pArg_2 = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                                           bytecode_[PC_ + 2]);
        arg_2 = builder_->CreateLoad(pArg_2);
    }
    else
        arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(),
                                       (char)bytecode_[PC_ + 2]);

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

    curCmpValue_ = builder_->CreateCmp(predicate, arg_1, arg_2, "resCmp");
    MovePC();
}

void Translator::Impl::TranslateByteCodeIO()
{
    std::vector<llvm::Type *> funcArgsTypes;
    funcArgsTypes.push_back(builder_->getInt8PtrTy());
    llvm::FunctionType *funcType =
                                 llvm::FunctionType::get(builder_->getInt32Ty(),
                                                         funcArgsTypes, true);

    std::string formatStr("%d");
    llvm::FunctionCallee func;
    llvm::Value* arg = nullptr;
    llvm::Value *pArg = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                                     bytecode_[PC_ + 1]);
    llvm::FunctionCallee printfReg = module_->getOrInsertFunction("printf",
                                                                  funcType);
    if (bytecode_[PC_] == WRITE) {
        arg = builder_->CreateLoad(pArg);
        func = printfReg;
        formatStr += "\n";
    }

    if (bytecode_[PC_] == READ) {
        arg = pArg;
        func = module_->getOrInsertFunction("scanf", funcType);
    }

    std::string regStr;
    switch (bytecode_[PC_ + 1]) {
    case RAX:
        regStr += "RAX: = ";
        break;
    case RBX:
        regStr += "RBX: = ";
        break;
    case RCX:
        regStr += "RCX: = ";
        break;
    case RDX:
        regStr += "RDX: = ";
        break;

    default:
        throw std::runtime_error("TranslateByteCodeIO():"
                                 "Undefined instruction" +
                                 std::to_string(bytecode_[PC_ + 1]));
    }
    llvm::Value *formatRegStrVal =
        builder_->CreateGlobalStringPtr(regStr, "IORegister");
    llvm::Value *formatStrVal =
        builder_->CreateGlobalStringPtr(formatStr, "IOformat");

    std::vector<llvm::Value *> args;
    args.push_back(formatRegStrVal);
    builder_->CreateCall(printfReg, args);

    args.clear();

    args.push_back(formatStrVal);
    args.push_back(arg);

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
        pArg = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                            bytecode_[PC_ + 1]);
        arg = builder_->CreateLoad(pArg);
        stackIR_.push(arg);
        break;

    case POP_R:
        pArg = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
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
    builder_->CreateRet(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));
    MovePC();
}

Translator::Translator(char *const pathToInputFile) :
    pImpl_(std::make_unique<Impl>(pathToInputFile)){};

Translator::~Translator() = default;

Translator::Translator(Translator &&) = default;

void Translator::Impl::ReadBytecode()
{
    FILE *inputFile = fopen(pathToInputFile_.c_str(), "rb");

    if (!inputFile)
        throw std::runtime_error("Simulator: Can`t open input file");

    fseek(inputFile, 0, SEEK_END);
    sizeByteCode_ = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    AllocByteCodeBuf();

    fread(bytecode_, 1, sizeByteCode_, inputFile);

    fclose(inputFile);
}

void Translator::Impl::AllocByteCodeBuf()
{
    bytecode_ = new unsigned char[sizeByteCode_];
}

void Translator::Impl::MovePC()
{
    switch (bytecode_[PC_]) {
    case ADD_R:
    case ADD:
    case SUB_R:
    case SUB:
    case IMUL_R:
    case IMUL:
    case IDIV_R:
    case IDIV:
    case MOV:
    case MOV_R:
    case CMP:
    case CMP_R:
        PC_ += 3;
        break;

    case JMP:
    case JG:
    case JGE:
    case JL:
    case JLE:
    case JE:
    case JNE:
    case WRITE:
    case READ:
    case PUSH:
    case PUSH_R:
    case POP_R:
    case CALL:
    case INC:
    case DEC:
        PC_ += 2;
        break;

    case RET:
    case EXIT:
        PC_ += 1;
        break;

    default:
        throw std::runtime_error("TranslateByteCode():Unidefined instruction" +
                                 std::to_string(bytecode_[PC_]));
    }
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

void Translator::Translate()
{
    pImpl_->Preprocess();
    // pImpl_->PreprocessBenchmark();
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
