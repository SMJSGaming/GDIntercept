#include "CodeLineCell.hpp"

CodeLineCell* CodeLineCell::create(const CCSize& size, const size_t lineNumber, const float lineNumberWidth, const HttpInfo::HttpContent& code, JSONTokenizer& tokenizer) {
    CodeLineCell* instance = new CodeLineCell(size, lineNumber, lineNumberWidth, code, tokenizer);

    if (instance && instance->init()) {
        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CodeLineCell::CodeLineCell(
    const CCSize& size,
    const size_t lineNumber,
    const float lineNumberWidth,
    const HttpInfo::HttpContent& code,
    JSONTokenizer& tokenizer
) : CullingCell(size),
m_code(code),
m_lineNumber(lineNumber),
m_lineNumberWidth(lineNumberWidth) {
    for (size_t index = m_code.contents.find('\t'); index != std::string::npos; index = m_code.contents.find('\t')) {
        m_code.contents.replace(index, 1, "  ");
    }

    if (code.type == HttpInfo::JSON) {
        m_tokens = tokenizer.parseLine(code.contents);
    }
}

void CodeLineCell::initRender() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    std::string truncatedCode = m_code.contents.size() >= 300 ? m_code.contents.substr(0, 300) : std::string(m_code.contents);
    CCLabelBMFont* lineNumberLabel = CCLabelBMFont::create(std::to_string(m_lineNumber).c_str(), theme.fontName);
    CCLabelBMFont* codeLabel = CCLabelBMFont::create(truncatedCode.c_str(), theme.fontName);

    if (m_tokens.empty()) {
        codeLabel->setColor(theme.text);
    } else for (const JSONTokenizer::TokenOffset& tokenOffset : m_tokens) {
        const size_t labelLength = truncatedCode.size();

        BREAK_WHEN(tokenOffset.offset >= labelLength);

        if (tokenOffset.offset >= labelLength) break;

        for (size_t i = tokenOffset.offset; i < truncatedCode.size() && i < tokenOffset.offset + tokenOffset.length; i++) {
            CCSprite* character = cocos::getChild<CCSprite>(codeLabel, i);

            switch (tokenOffset.token) {
                case JSONTokenizer::CORRUPT: CASE_BREAK(character->setColor(theme.error));
                case JSONTokenizer::KEY: CASE_BREAK(character->setColor(theme.key));
                case JSONTokenizer::STRING: CASE_BREAK(character->setColor(theme.string));
                case JSONTokenizer::NUMBER: CASE_BREAK(character->setColor(theme.number));
                case JSONTokenizer::CONSTANT: CASE_BREAK(character->setColor(theme.constant));
                default: CASE_BREAK(character->setColor(theme.text));
            }
        }

    }

    lineNumberLabel->setScale(theme.fontSize);
    lineNumberLabel->setAnchorPoint(BOTTOM_RIGHT);
    lineNumberLabel->setColor(theme.lineNum);
    lineNumberLabel->setPosition({ m_lineNumberWidth, theme.lineHeight / 2 });
    codeLabel->setScale(theme.fontSize);
    codeLabel->setAnchorPoint(BOTTOM_LEFT);
    codeLabel->setPosition({ m_lineNumberWidth + PADDING, theme.lineHeight / 2 });
    m_mainLayer->addChild(lineNumberLabel);
    m_mainLayer->addChild(codeLabel);
}