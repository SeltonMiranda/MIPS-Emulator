#ifndef _TOKENIZER_
#define _TOKENIZER_

#include <fstream>
#include <format>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace Emulator
{
    enum class Type
    {
        INSTRUCTION,
        ARG,
    };

    struct Token // (Unresolved token)
    {
        Type type;
        size_t line; // line that token was in it
        std::string value;
        Token(Type type, size_t line, const std::string &value);
    };

    struct ResolvedToken
    {
        Type type;
        size_t line;
        std::string value;
        std::uint64_t address;
        std::uint64_t args;

        ResolvedToken(Type type, size_t line, const std::string &value, std::uint64_t address);
    };

    class Tokenizer
    {
    private:
        std::ifstream file;
        std::vector<Token *> resolvedTokens;
        size_t current_line;
        // Parses a line
        auto parseLine(std::vector<Token*> &tokens, std::string &line, size_t lineNumber) -> void;

    public:
        Tokenizer(std::ifstream &&file);
        Tokenizer(const std::string &file);
        ~Tokenizer();

        // Parses a file, returns an array of (unresolved) tokens
        auto parse() -> std::vector<Token*>;

        auto resolveTokens(const std::vector<Token*>& tokens) -> void; // todo

        // Debug purposes
        auto printTokens() -> void;
    };

}

#endif // _TOKENIZER_