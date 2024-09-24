#include "JSONTokenizer.hpp"

JSONTokenizer::JSONTokenizer() : m_token(Token::UNKNOWN), m_futureToken(Token::UNKNOWN), m_constantSize(0) {
    this->reset();
}

std::vector<JSONTokenizer::TokenOffset> JSONTokenizer::parseLine(const std::string& code) {
    const size_t length = code.size();
    bool previousWasAccepted = true;
    std::vector<TokenOffset> tokens;

    if (m_token == Token::STRING || m_token == Token::STRING_QUOTE || m_token == Token::NUMBER || m_token == Token::CONSTANT) {
        m_token = m_futureToken = Token::TERMINATOR;

        this->reset();
    } else if ((m_token == Token::KEY || m_token == Token::KEY_QUOTE) && m_openQuote != 0) {
        m_token = m_futureToken = Token::SEPARATOR;

        this->reset();
    }

    tokens.push_back({ 0, 0, m_token });

    for (size_t i = 0; i < length; i++) {
        TokenOffset& back = tokens.back();

        if (code[i] != ' ' || (m_token == Token::STRING || m_token == Token::NUMBER)) {
            if (previousWasAccepted && m_token == Token::STRING && !m_escape && back.token == m_token) {
                back.length = (i = code.find_first_of(std::string() + m_openQuote + '\\', i)) - back.offset;
            }

            previousWasAccepted = this->determineCharToken(code[i], code.substr(i), previousWasAccepted);

            if (previousWasAccepted) {
                if (back.token == m_token) {
                    back.length++;
                } else {
                    tokens.push_back(TokenOffset({ i, 1, m_token }));
                }
            } else if (back.token == Token::CORRUPT) {
                back.length++;
            } else {
                tokens.push_back(TokenOffset({ i, 1, Token::CORRUPT }));
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
    if (m_token == Token::SEPARATOR) {
        if (character == ':') {
            m_futureToken = Token::UNKNOWN;

            return true;
        } else {
            return false;
        }
    } else if (m_token == Token::KEY || m_token == Token::KEY_QUOTE) {
        if (m_openQuote != 0) {
            return this->checkString(character, truncatedCode.size());
        } else if (character == '\'' || character == '"') {
            m_openQuote = character;
            m_futureToken = Token::KEY;

            return true;
        } else {
            return this->checkObjectTerminator(character);
        }
    } else {
        bool accepted = true;

        switch (m_token) {
            case Token::STRING: accepted = !previousWasAccepted || this->checkString(character, truncatedCode.size()); break;
            case Token::NUMBER: if (character == ' ') {
                m_token = m_futureToken = Token::TERMINATOR;
            } else if (!previousWasAccepted || (!isdigit(character) && (character != '.' || m_hasDecimal))) {
                accepted = false;
            } else if (character == '.') {
                m_hasDecimal = true;
                accepted = truncatedCode.size() > 1;
            } break;
            case Token::CONSTANT: if (m_constantSize == ++m_constantCounter) {
                m_futureToken = Token::TERMINATOR;
            } break;
            case Token::UNKNOWN: {
                const bool startsWithFalse = truncatedCode.starts_with("false");

                if (startsWithFalse || truncatedCode.starts_with("true") || truncatedCode.starts_with("null")) {
                    m_token = m_futureToken = Token::CONSTANT;
                    m_constantSize = 4 + startsWithFalse;
                } else if (character == '"' || character == '\'') {
                    m_token = Token::STRING_QUOTE;
                    m_futureToken = Token::STRING;
                    m_openQuote = character;
                } else if (isdigit(character) || character == '-') {
                    m_token = m_futureToken = Token::NUMBER;
                } else if (character == '{') {
                    m_token = Token::BRACKET;
                    m_futureToken = Token::KEY_QUOTE;
                    m_closesExpected += '}';
                } else if (character == '[') {
                    m_token = Token::BRACKET;
                    m_futureToken = Token::UNKNOWN;
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
            m_token = Token::TERMINATOR;
            m_futureToken = m_closesExpected.ends_with('}') ? Token::KEY_QUOTE : Token::UNKNOWN;

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
        if (m_token == Token::KEY) {
            m_token = Token::KEY_QUOTE;
            m_futureToken = Token::SEPARATOR;
        } else {
            m_token = Token::STRING_QUOTE;
            m_futureToken = Token::TERMINATOR;
        }
    } else if (charactersLeft > 1) {
        m_escape = false;
    } else {
        return false;
    }

    return true;
}

bool JSONTokenizer::checkObjectTerminator(const char character) {
    if (m_closesExpected.ends_with(character)) {
        m_token = Token::BRACKET;
        m_futureToken = Token::TERMINATOR;
        m_closesExpected.pop_back();

        return true;
    } else {
        return false;
    }
}

void JSONTokenizer::reset() {
    if (m_futureToken != Token::STRING && m_futureToken != Token::KEY) {
        m_openQuote = 0;
    }

    m_escape = false;
    m_hasDecimal = false;
    m_token = m_futureToken;
    m_constantCounter = 1;
}