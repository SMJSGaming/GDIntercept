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

float CodeLineCell::getCodeLineWidth() const {
    return m_lineNumberWidth + PADDING;
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

    if (code.type == ContentType::JSON) {
        m_tokens = tokenizer.parseLine(code.contents);
    }
}

void CodeLineCell::initRender() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const float codeLineWidth = this->getCodeLineWidth();
    CCLabelBMFont* lineNumberLabel = CCLabelBMFont::create(std::to_string(m_lineNumber).c_str(), theme.font.fontName);
    CCLabelBMFont* codeLabel = CCLabelBMFont::create(
        m_code.contents.substr(0, std::min<size_t>(m_code.contents.size(), 2000)).c_str(),
        theme.font.fontName
    );
    CCLayerColor* numberBackground = CCLayerColor::create(theme.ui.background, codeLineWidth, this->getContentHeight());

    if (m_tokens.empty()) {
        codeLabel->setColor(theme.syntax.text);
    } else for (const JSONTokenizer::TokenOffset& tokenOffset : m_tokens) {
        const size_t labelLength = m_code.contents.size();

        BREAK_WHEN(tokenOffset.offset >= labelLength);

        for (size_t i = tokenOffset.offset; i < labelLength && i < tokenOffset.offset + tokenOffset.length; i++) {
            CCSprite* character = cocos::getChild<CCSprite>(codeLabel, i);

            CONTINUE_WHEN(!character);

            switch (tokenOffset.token) {
                case JSONTokenizer::Token::CORRUPT: character->setColor(theme.syntax.error); break;
                case JSONTokenizer::Token::KEY: character->setColor(theme.syntax.key); break;
                case JSONTokenizer::Token::STRING: character->setColor(theme.syntax.string); break;
                case JSONTokenizer::Token::NUMBER: character->setColor(theme.syntax.number); break;
                case JSONTokenizer::Token::CONSTANT: character->setColor(theme.syntax.constant); break;
                case JSONTokenizer::Token::TERMINATOR: character->setColor(theme.syntax.terminator); break;
                case JSONTokenizer::Token::SEPARATOR: character->setColor(theme.syntax.separator); break;
                case JSONTokenizer::Token::BRACKET: character->setColor(theme.syntax.bracket); break;
                default: character->setColor(theme.syntax.text); break;
            }
        }
    }

    lineNumberLabel->setScale(theme.font.fontScale);
    lineNumberLabel->setAnchorPoint(BOTTOM_RIGHT);
    lineNumberLabel->setColor(theme.ui.foreground);
    lineNumberLabel->setPosition({ m_lineNumberWidth, theme.font.lineHeight / 2 });
    codeLabel->setID("code");
    codeLabel->setScale(theme.font.fontScale);
    codeLabel->setAnchorPoint(BOTTOM_LEFT);
    codeLabel->setPosition({ codeLineWidth, theme.font.lineHeight / 2 });
    numberBackground->setID("horizontal-align");
    numberBackground->setAnchorPoint(BOTTOM_LEFT);
    numberBackground->setPosition(ZERO_POINT);
    numberBackground->setZOrder(1);
    numberBackground->addChild(lineNumberLabel);
    m_mainLayer->addChild(numberBackground);
    m_mainLayer->addChild(codeLabel);
}