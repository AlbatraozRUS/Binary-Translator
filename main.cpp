#include "Assembler/Assembler.h"
#include "Simulator/Simulator.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: Incorrect number of arguments\n";
        exit(EXIT_FAILURE);
    }

    Assembler assembler(argv[1], argv[2]);                

    try {
        assembler.Assemble();
    }

    catch (std::exception &exception)
    {
        std::cerr << exception.what() << "\n";
        exit(EXIT_FAILURE);
    }
    //assembler.Dump()

    CpuSimulator cpuSimulator;

    try {
        cpuSimulator.Run(argv[2]);
    }

    catch (std::exception &exception)
    {
        std::cerr << exception.what() << "\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}