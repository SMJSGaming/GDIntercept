#include "JSONColor.hpp"

JSONColor::JSONColor() : m_token(UNKNOWN), m_futureToken(UNKNOWN), m_line(0), m_constantSize(0) {
    this->reset();
}

void JSONColor::parseLine(const std::string& code, CCLabelBMFont* label) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const size_t labelLength = label->getChildrenCount();
    const size_t length = code.size();
    bool previousWasAccepted = true;

    if (m_token == STRING || m_token == NUMBER || m_token == CONSTANT) {
        m_token = m_futureToken = TERMINATOR;

        this->reset();
    } else if (m_token == KEY && m_openQuote != 0) {
        m_token = m_futureToken = KEY_TERMINATOR;

        this->reset();
    }

    for (size_t i = 0; i < length; i++) {
        if (code[i] != ' ' || (m_token == STRING || m_token == NUMBER)) {
            if (previousWasAccepted && m_token == STRING) {
                const size_t originalI = i;
                const size_t end = std::min(i = code.find_first_of(std::string() + m_openQuote + '\\', i), labelLength);

                for (size_t j = originalI; j < end; j++) {
                    cocos::getChild<CCSprite>(label, j)->setColor(theme.string);
                }
            }

            CCSprite* child = cocos::getChild<CCSprite>(label, i);

            previousWasAccepted = this->determineCharToken(code[i], code.substr(i), previousWasAccepted);

            if (child) {
                switch (m_token) {
                    case KEY: child->setColor(theme.key); break;
                    case STRING: child->setColor(theme.string); break;
                    case NUMBER: child->setColor(theme.number); break;
                    case CONSTANT: child->setColor(theme.constant); break;
                    default: child->setColor(theme.text);
                }

                if (!previousWasAccepted) {
                    child->setColor(theme.error);
                }
            }

            if (m_futureToken != m_token) {
                this->reset();
            }
        }
    }

    m_line++;
}

bool JSONColor::determineCharToken(const char character, const std::string& truncatedCode, const bool previousWasAccepted) {
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
            case STRING: {
                accepted = !previousWasAccepted || this->checkString(character, truncatedCode.size());
            } break;
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
            default: {
                accepted = false;
            }
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

bool JSONColor::checkString(const char character, const size_t charactersLeft) {
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

bool JSONColor::checkObjectTerminator(const char character) {
    if (m_closesExpected.ends_with(character)) {
        m_token = m_futureToken = TERMINATOR;
        m_closesExpected.pop_back();

        return true;
    } else {
        return false;
    }
}

void JSONColor::reset() {
    m_openQuote = 0;
    m_escape = false;
    m_hasDecimal = false;
    m_token = m_futureToken;
    m_constantCounter = 1;
}