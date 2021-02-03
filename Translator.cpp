#include "Translator.h"

#include <iostream>
#include <fstream>
#include <stack>

class Translator::Impl {
private:
    std::string pathToInputFile_;

    char* bytecode_ = nullptr;
    size_t sizeByteCode_ = 0;
    size_t PC_ = 0;
    std::string output_;

    llvm::LLVMContext context_;
    llvm::Module* module_ = nullptr;
    llvm::IRBuilder<>* builder_ = nullptr;
    //std::stack<llvm::Value> stackIR_;

    void TranslateByteCode();
    void ReadBytecode();
    void AllocByteCodeBuf();

    void TranslateByteCodeMath();
    void TranslateByteCodeJumps();
    void TranslateByteCodeCmp();
    void TranslateByteCodeIO();
    void TranslateByteCodeMem();    
    void TranslateByteCodeCall();
    void TranslateByteCodeRet();

public:
 Impl(char *const pathToInputFile) : pathToInputFile_(pathToInputFile){}

    ~Impl()
    {
        delete [] bytecode_;
    }
    
    void Translate();

    friend void Translator::Dump() const;
}; //class Translator::Impl

void Translator::Impl::Translate()
{
    ReadBytecode();

    module_ = new llvm::Module("top", context_);
    builder_ = new llvm::IRBuilder(context_);

    llvm::FunctionType *funcType = llvm::FunctionType::get(builder_->getInt32Ty(), false);
    llvm::Function *mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module_);

    llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context_, "entry", mainFunc);

    builder_->SetInsertPoint(entryBB);

    TranslateByteCode();

    builder_->CreateRet(llvm::ConstantInt::get(builder_->getInt32Ty(), 0));
}

void Translator::Impl::TranslateByteCode()
{
    while(PC_ < sizeByteCode_)
        switch(bytecode_[PC_]){
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
                TranslateByteCodeMath();
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
            case MOV:
            case MOV_R:
                TranslateByteCodeMem();
                break;

            case CALL:
                TranslateByteCodeCall();
                break;

            case RET:
                TranslateByteCodeRet();
                break;

            case EXIT:
                break;                                     

            default: throw std::runtime_error
                ("Unidefined instruction" + std::to_string(bytecode_[PC_]));
        }
}

void Translator::Impl::TranslateByteCodeMath()
{

}

void Translator::Impl::TranslateByteCodeJumps()
{

}

void Translator::Impl::TranslateByteCodeCmp()
{

}

void Translator::Impl::TranslateByteCodeIO()
{

}

void Translator::Impl::TranslateByteCodeMem()
{

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
    bytecode_ = new char[sizeByteCode_];
}

void Translator::Translate()
{
    pImpl_->Translate();
}

void Translator::Dump() const
{
    std::cout << "#[LLVM_IR]:\n";
    std::string s;
    llvm::raw_string_ostream os(s);
    pImpl_->module_->print(os, nullptr);
    os.flush();
    std::cout << s;
}
