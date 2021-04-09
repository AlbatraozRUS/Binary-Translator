#include "Translator.h"

#include <iostream>

// namespace {
//     void ReadFromInput(char *pathToFile, std::string &inputText, bool isFile)
//     {
//         if (isFile)
//         {
//             std::ifstream inputFile(pathToFile, std::ios_base::in | std::ios_base::binary);

//             if (!inputFile)
//             {
//                 std::cerr << "Error: Incorrect path to input file!\n";
//                 exit(EXIT_FAILURE);
//             }

//             while (inputFile)
//                 inputFile >> inputText;

//             inputFile.close();
//         }
//         else
//             while (getline(std::cin, inputText, '\0'))
//             {
//             }
//     }
// }

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "Error: Incorrect input\n");
        exit(EXIT_FAILURE);
    }

    Translator translator(argv[1]);
    translator.Translate();
    translator.Dump();

    return 0;
}
