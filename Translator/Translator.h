#ifndef BINARY_TRANSLATOR_TRANSLATOR_H
#define BINARY_TRANSLATOR_TRANSLATOR_H

#include <experimental/propagate_const>
#include <memory>

class Translator {
private:
    class Impl;
    std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl_;

public:

    Translator(char *const pathToInputFile);

    Translator(const Translator &) = delete;
    Translator &operator=(const Translator &) = delete;
    Translator(Translator &&);

    ~Translator();

    void Translate();

    void Dump() const;
};


#endif //BINARY_TRANSLATOR_TRANSLATOR_H