#include "Translator.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <map>

namespace
{
    bool IsRegRegInst(const int inst)
    {
        if (inst == ADD_R || inst == SUB_R ||
            inst == IMUL_R || inst == IDIV_R ||
            inst == CMP_R || inst == MOV_R)
            return true;

        return false;
    }
} // namespace

class Translator::Impl
{
private:
    std::string pathToInputFile_;

    unsigned char *bytecode_ = nullptr;
    size_t sizeByteCode_ = 0;
    size_t PC_ = 0;
    std::string output_;

    llvm::LLVMContext context_;
    llvm::Module *module_ = nullptr;
    llvm::IRBuilder<> *builder_ = nullptr;
    llvm::GlobalVariable *regs_ = nullptr;
    llvm::ArrayType *regsType_ = nullptr;

    std::stack<llvm::Value *> stackIR_;

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

public:
    Impl(char *const pathToInputFile) : pathToInputFile_(pathToInputFile) {}

    ~Impl()
    {
        delete[] bytecode_;
    }

    void Translate();

    friend void Translator::Dump() const;

}; // class Translator::Impl

void Translator::Impl::Translate()
{
    ReadBytecode();

    // Create basic
    module_ = new llvm::Module("top", context_);
    builder_ = new llvm::IRBuilder(context_);

    // Create global array of registers
    regsType_ = llvm::ArrayType::get(builder_->getInt32Ty(), NREGS);
    module_->getOrInsertGlobal("regs_", regsType_);
    regs_ = module_->getNamedGlobal("regs_");

    std::vector<llvm::Constant *> temp;
    for (size_t i = 0; i < NREGS; i++)
        temp.push_back(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));

    regs_->setInitializer(llvm::ConstantArray::get(regsType_, temp));

    llvm::FunctionType *funcType = llvm::FunctionType::get(builder_->getInt32Ty(), false);
    llvm::Function *mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module_);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context_, "entry", mainFunc);

    builder_->SetInsertPoint(entryBB);

    TranslateByteCode();

    builder_->CreateRet(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));
}

void Translator::Impl::TranslateByteCode()
{
    while (PC_ < sizeByteCode_) 
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
            break;

        default:
            throw std::runtime_error("TranslateByteCode():Unidefined instruction" + std::to_string(bytecode_[PC_]));
        }
}

void Translator::Impl::TranslateByteCodeExpression()
{

    llvm::Value *pArg_1 = builder_->CreateConstGEP2_32(regsType_, regs_, 0, bytecode_[PC_ + 1]);
    llvm::Value *arg_1 = builder_->CreateLoad(pArg_1);

    llvm::Value *arg_2 = nullptr;
    if (IsRegRegInst(bytecode_[PC_]))
    {
        llvm::Value *pArg_2 = builder_->CreateConstGEP2_32(regsType_, regs_, 0,
                                                           bytecode_[PC_ + 2]);
        arg_2 = builder_->CreateLoad(pArg_2);
    }
    else
        arg_2 = llvm::ConstantInt::get(builder_->getInt32Ty(), (char)bytecode_[PC_ + 2]);

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
        throw std::runtime_error("TranslateByteCodeExpression(): Unidefined instruction" + std::to_string(bytecode_[PC_]));
    }

    builder_->CreateStore(res, pArg_1);

    if (bytecode_[PC_] == INC || bytecode_[PC_] == DEC)
        PC_ += 2;
    else
        PC_ += 3;
}

void Translator::Impl::TranslateByteCodeJumps()
{
}

void Translator::Impl::TranslateByteCodeCmp()
{
}

void Translator::Impl::TranslateByteCodeIO()
{
    std::vector<llvm::Type *> funcArgsType;
    funcArgsType.push_back(builder_->getInt8PtrTy());

    auto funcType = llvm::FunctionType::get(builder_->getInt32Ty(), funcArgsType, true);
    auto formatVal = builder_->CreateGlobalStringPtr("%d", "IOformat");
    
    std::vector<llvm::Value *> args;
    args.push_back(formatVal);
    Value *pArg = builder_->CreateConstGEP2_32(regsType_, regs_, 0, bytecode_[PC_ + 1]);

    Function *func = nullptr;
    Value *arg = nullptr;
    if (bytecode_[PC_] == WRITE)
    {
        arg = builder_->CreateLoad(pArg);
        func = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage,
                                      "printf", module_);
    }

    if (bytecode_[PC_] == READ)
    {
        arg = pArg;
        func = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage,
                                      "scanf", module_);
    }
    args.push_back(arg);
    builder_->CreateCall(func, args);

    PC_ += 2;
}

void Translator::Impl::TranslateByteCodeStack()
{
    llvm::Value* pArg = nullptr;
    llvm::Value* arg = nullptr;

    // Todo check empty 
    switch (bytecode_[PC_]) {
    case PUSH:
        arg = llvm::ConstantInt::get(builder_->getInt32Ty(), (char)bytecode_[PC_ + 1]);
        stackIR_.push(arg);
        break;

    case PUSH_R:
        pArg = builder_->CreateConstGEP2_32(regsType_, regs_, 0, bytecode_[PC_ + 1]);
        arg = builder_->CreateLoad(pArg);
        stackIR_.push(arg);
        break;
    
    case POP_R:
        pArg = builder_->CreateConstGEP2_32(regsType_, regs_, 0, bytecode_[PC_ + 1]);
        arg = stackIR_.top();
        builder_->CreateStore(arg, pArg);
        stackIR_.pop();
        break;

    default:
        throw std::runtime_error("TranslateByteCodeStack(): Unidefined instruction" 
                                 + std::to_string(bytecode_[PC_]));
    } 

    PC_ += 2;
}

void Translator::Impl::TranslateByteCodeCall()
{
}

void Translator::Impl::TranslateByteCodeRet()
{
}

Translator::Translator(char *const pathToInputFile) : pImpl_(std::make_unique<Impl>(pathToInputFile)){};

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

void Translator::Translate()
{
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
