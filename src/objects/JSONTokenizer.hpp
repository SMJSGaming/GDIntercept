#pragma once

#include "../include.hpp"

class JSONTokenizer {
public:
    enum class Token {
        UNKNOWN,
        CORRUPT,
        KEY,
        STRING,
        NUMBER,
        CONSTANT,
        TERMINATOR,
        SEPARATOR,
        BRACKET,
        KEY_QUOTE,
        STRING_QUOTE
    };

    struct TokenOffset {
        size_t offset;
        size_t length;
        Token token;
    };

    JSONTokenizer();
    std::vector<TokenOffset> parseLine(const std::string& code);
private:
    Token m_token;
    Token m_futureToken;
    char m_openQuote;
    bool m_escape;
    bool m_hasDecimal;
    size_t m_constantSize;
    size_t m_constantCounter;
    std::string m_closesExpected;

    bool determineCharToken(const char character, const std::string& truncatedCode, const bool previousWasAccepted);
    bool checkString(const char character, const size_t charactersLeft);
    bool checkObjectTerminator(const char character);
    void reset();
};