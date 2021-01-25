#include <fstream>
#include <iostream>
#include <cstdlib>

void ReadFromInput(char* pathToFile, std::string& inputText, bool isFile);

int main(int argc, char **argv)
{
    if (argc == 1){
        fprintf(stderr, "Error: Incorrect input\n");
        exit(EXIT_FAILURE);
    }

    std::string inputText;

    switch(argv[1][1]){
        case 'f': {ReadFromInput(argv[2], inputText, true); break;}

        case 'i': {ReadFromInput(nullptr, inputText, false); break;}

        default:  {std::cerr << "Error: Incorrect argument\n"; abort();}
    }

    return 0;
}

void ReadFromInput(char* pathToFile, std::string& inputText, bool isFile)
{            
    if (isFile){    
        std::ifstream inputFile(pathToFile);

        if (!inputFile){
            std::cerr << "Error: Incorrect path to input file!\n";
            exit(EXIT_FAILURE);
        }
        
        while (inputFile)
            inputFile >> inputText;

        inputFile.close();        
    }
    else
        while (getline(std::cin, inputText, '\0')){}        
}
