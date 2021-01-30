#include "Assembler.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: Incorrect number of arguments\n";
        exit(EXIT_FAILURE);
    }

    Assembler assembler;

    try {assembler.Assemble(argv[1]);}
    catch (std::exception &exception)
    {
        std::cerr << exception.what() << "\n";
        exit(EXIT_FAILURE);
    }

    assembler.Dump();

    return 0;
}