#include "JSONTokenizer.hpp"

JSONTokenizer::JSONTokenizer() : m_token(UNKNOWN), m_futureToken(UNKNOWN), m_constantSize(0) {
    this->reset();
}

std::vector<JSONTokenizer::TokenOffset> JSONTokenizer::parseLine(const std::string& code) {
    const size_t length = code.size();
    bool previousWasAccepted = true;
    std::vector<TokenOffset> tokens;

    if (m_token == STRING || m_token == NUMBER || m_token == CONSTANT) {
        m_token = m_futureToken = TERMINATOR;

        this->reset();
    } else if (m_token == KEY && m_openQuote != 0) {
        m_token = m_futureToken = KEY_TERMINATOR;

        this->reset();
    }

    tokens.push_back({ 0, 0, m_token });

    for (size_t i = 0; i < length; i++) {
        TokenOffset& back = tokens.back();

        if (code[i] != ' ' || (m_token == STRING || m_token == NUMBER)) {
            if (previousWasAccepted && m_token == STRING) {
                back.length = (i = code.find_first_of(std::string() + m_openQuote + '\\', i)) - back.offset;
            }

            previousWasAccepted = this->determineCharToken(code[i], code.substr(i), previousWasAccepted);

            if (previousWasAccepted) {
                if (back.token == m_token) {
                    back.length++;
                } else {
                    tokens.push_back(TokenOffset({ i, 1, m_token }));
                }
            } else if (back.token == CORRUPT) {
                back.length++;
            } else {
                tokens.push_back(TokenOffset({ i, 1, CORRUPT }));
            }

            if (m_futureToken != m_token) {
                this->reset();
            }
        } else {
            back.length++;
        }
    }

    return tokens;
}

bool JSONTokenizer::determineCharToken(const char character, const std::string& truncatedCode, const bool previousWasAccepted) {
    if (m_token == KEY_TERMINATOR) {
        if (character == ':') {
            m_futureToken = UNKNOWN;

            return true;
        } else {
            return false;
        }
    } else if (m_token == KEY) {
        if (m_openQuote != 0) {
            return this->checkString(character, truncatedCode.size());
        } else if (character == '\'' || character == '"') {
            m_openQuote = character;

            return true;
        } else {
            return this->checkObjectTerminator(character);
        }
    } else {
        bool accepted = true;

        switch (m_token) {
            case STRING: accepted = !previousWasAccepted || this->checkString(character, truncatedCode.size()); break;
            case NUMBER: if (character == ' ') {
                m_token = m_futureToken = TERMINATOR;
            } else if (!previousWasAccepted || (!isdigit(character) && (character != '.' || m_hasDecimal))) {
                accepted = false;
            } else if (character == '.') {
                m_hasDecimal = true;
                accepted = truncatedCode.size() > 1;
            } break;
            case CONSTANT: if (m_constantSize == ++m_constantCounter) {
                m_futureToken = TERMINATOR;
            } break;
            case UNKNOWN: {
                const bool startsWithFalse = truncatedCode.starts_with("false");

                if (startsWithFalse || truncatedCode.starts_with("true") || truncatedCode.starts_with("null")) {
                    m_token = m_futureToken = CONSTANT;
                    m_constantSize = 4 + startsWithFalse;
                } else if (character == '"' || character == '\'') {
                    m_token = m_futureToken = STRING;
                    m_openQuote = character;
                } else if (isdigit(character) || character == '-') {
                    m_token = m_futureToken = NUMBER;
                } else if (character == '{') {
                    m_futureToken = KEY;
                    m_closesExpected += '}';
                } else if (character == '[') {
                    m_closesExpected += ']';
                } else {
                    accepted = character == ' ';
                }
            } break;
            default: accepted = false; break;
        }

        if (accepted || m_openQuote != 0) {
            return accepted;
        } else if (character == ',') {
            m_token = TERMINATOR;
            m_futureToken = m_closesExpected.ends_with('}') ? KEY : UNKNOWN;

            return true;
        } else {
            return this->checkObjectTerminator(character);
        }
    }
}

bool JSONTokenizer::checkString(const char character, const size_t charactersLeft) {
    if (character == '\\') {
        m_escape = !m_escape;
    } else if (character == m_openQuote && !m_escape) {
        m_futureToken = m_token == KEY ? KEY_TERMINATOR : TERMINATOR;
    } else if (charactersLeft > 1) {
        m_escape = false;
    } else {
        return false;
    }

    return true;
}

bool JSONTokenizer::checkObjectTerminator(const char character) {
    if (m_closesExpected.ends_with(character)) {
        m_token = m_futureToken = TERMINATOR;
        m_closesExpected.pop_back();

        return true;
    } else {
        return false;
    }
}

void JSONTokenizer::reset() {
    m_openQuote = 0;
    m_escape = false;
    m_hasDecimal = false;
    m_token = m_futureToken;
    m_constantCounter = 1;
}