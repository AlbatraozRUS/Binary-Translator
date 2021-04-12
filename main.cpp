
#include "Assembler.h"
#include "Simulator.h"
#include "Translator.h"

//TODO refactor .gitignore

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Error: Incorrect number of arguments\n";
        exit(EXIT_FAILURE);
    }

    try {
        BinaryTranslator::Assembler assembler(argv[1], argv[2]);
        assembler.Assemble();
        // assembler.Dump();
    }
    catch (std::exception &exception) {
        std::cerr << exception.what() << "\n";
        exit(EXIT_FAILURE);
    }

    // try {
    //     BinaryTranslator::CpuSimulator cpuSimulator;
    //     cpuSimulator.Run(argv[2]);
    // }
    // catch (std::exception &exception) {
    //     std::cerr << exception.what() << "\n";
    //     exit(EXIT_FAILURE);
    // }


    try {
        BinaryTranslator::Translator translator(argv[2], true);
        translator.Translate();
        translator.Dump();
    }
    catch(std::runtime_error& exception){
        std::cerr << exception.what();
    }

    return 0;
}
