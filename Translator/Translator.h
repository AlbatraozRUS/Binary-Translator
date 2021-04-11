#ifndef BINARY_TRANSLATOR_TRANSLATOR_H
#define BINARY_TRANSLATOR_TRANSLATOR_H

#include <experimental/propagate_const>
#include <memory>

namespace BinaryTranslator {

class Translator {
private:
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl_;

public:

    Translator(char* pathToInputFile, bool isAnalyse = false);

    Translator(const Translator &) = delete;
    Translator &operator=(const Translator &) = delete;
    Translator(Translator &&);

    ~Translator();

    void Translate();

    void Dump() const;
};

} //namespace BinaryTranslator

#endif //BINARY_TRANSLATOR_TRANSLATOR_H