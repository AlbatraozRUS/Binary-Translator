#include "Assembler.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: Incorrect number of arguments\n";
        exit(EXIT_FAILURE);
    }

    Assembler assembler(argv[1]);    
    if (argc == 3){        
        assembler.SetOutputFile(argv[2]);
    }

    try {assembler.Assemble();}
    catch (std::exception &exception)
    {
        std::cerr << exception.what() << "\n";
        exit(EXIT_FAILURE);
    }

    //assembler.Dump();

    return 0;
}