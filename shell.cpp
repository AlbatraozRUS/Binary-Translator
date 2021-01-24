#include <fstream>
#include <iostream>
#include <cstdlib>

void ReadFromFile(char *pathToFile, std::string &inputText, bool isFileInput);

int main(int argc, char **argv)
{
    if (argc == 1){
        fprintf(stderr, "Error: Incorrect input\n");
        exit(EXIT_FAILURE);
    }

    std::string inputText;

    switch(argv[1][1]){
        case 'f': {ReadFromFile(argv[2], inputText, true); break;}

        case 'i': {ReadFromFile(nullptr, inputText, false); break;}

        default:  {std::cerr << "Error: Incorrect argument\n"; abort();}
    }

    return 0;
}

void ReadFromFile(char* pathToFile, std::string& inputText, bool isFileInput)
{            
    if (isFileInput){    
        std::ifstream inputFile(pathToFile);

        if (!inputFile){
            std::cerr << "Error: Incorrect path to input file!\n";
            exit(EXIT_FAILURE);
        }
        
        while (inputFile)
            inputFile >> inputText;

        inputFile.close();        
    }
    else {
        while (getline(std::cin, inputText)){}    
        std::cout << inputText;    
    }
}
