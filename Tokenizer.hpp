#ifndef _TOKENIZER_
#define _TOKENIZER_

#include <fstream>
#include <format>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace Emulator {

struct Token {
    enum class Type {
        Register,
        Instruction,
        Unknown
    };

    Type _type;
    size_t line; // line that token was in it
};

class Tokenizer
{
    private:
        std::ifstream program;
        std::vector<Token> tokens;
        size_t current_line;

    public:
        Tokenizer(std::ifstream&& file);

        Tokenizer(const std::string& file);

        ~Tokenizer();

        auto parse() -> std::vector<std::string>;

        // Only for debug
        auto printSymbols(const std::vector<std::string>& symbols) -> void;
};

}


#endif // _TOKENIZER_