#pragma once

#include "../include.hpp"
#include "../objects/ThemeStyle.hpp"

struct JSONColor {
    enum Token {
        UNKNOWN,
        KEY,
        STRING,
        NUMBER,
        CONSTANT,
        TERMINATOR,
        KEY_TERMINATOR
    };

    JSONColor();
    void parseLine(const std::string& code, CCLabelBMFont* label);
private:
    Token m_token;
    Token m_futureToken;
    char m_openQuote;
    bool m_escape;
    bool m_hasDecimal;
    size_t m_line;
    size_t m_constantSize;
    size_t m_constantCounter;
    std::string m_closesExpected;

    bool determineCharToken(const char character, const std::string& truncatedCode, const bool previousWasAccepted);
    bool checkString(const char character, const size_t charactersLeft);
    bool checkObjectTerminator(const char character);
    void reset();
};