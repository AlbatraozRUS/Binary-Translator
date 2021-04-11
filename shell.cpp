#include "Translator.h"

#include <iostream>

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "Error: Incorrect input\n");
        exit(EXIT_FAILURE);
    }

    try {
    Translator translator(argv[1]);
    translator.Translate();
    translator.Dump();
    }
    catch(std::runtime_error& exception){
        std::cerr << exception.what();
    }

    return 0;
}
