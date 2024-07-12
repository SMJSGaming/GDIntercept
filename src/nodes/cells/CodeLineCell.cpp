#include "CodeLineCell.hpp"

CodeLineCell* CodeLineCell::create(const HttpInfo::HttpContent& code, const size_t lineNumber, const float lineNumberWidth, JSONColor& color) {
    CodeLineCell* instance = new CodeLineCell(lineNumberWidth, color);

    if (instance && instance->init(code.type, code.contents, lineNumber)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CodeLineCell::CodeLineCell(const float lineNumberWidth, JSONColor& color) : m_lineNumberWidth(lineNumberWidth), m_color(color) { }

bool CodeLineCell::init(const HttpInfo::ContentType content, std::string code, const size_t lineNumber) {
    if (!CCLayer::init()) {
        return false;
    }

    for (size_t index = code.find('\t'); index != std::string::npos; index = code.find('\t')) {
        code.replace(index, 1, "  ");
    }

    const ThemeStyle& theme = ThemeStyle::getTheme();
    CCLabelBMFont* lineNumberLabel = CCLabelBMFont::create(std::to_string(lineNumber).c_str(), theme.fontName);
    // Fuck it, we're truncating this bitch
    CCLabelBMFont* codeLabel = CCLabelBMFont::create((code.size() >= 300 ? code.substr(0, 297) + "..." : code).c_str(), theme.fontName);

    if (content == HttpInfo::JSON) {
        m_color.parseLine(code, codeLabel);
    } else {
        codeLabel->setColor(theme.text);
    }

    lineNumberLabel->setScale(theme.fontSize);
    lineNumberLabel->setAnchorPoint(BOTTOM_RIGHT);
    lineNumberLabel->setColor(theme.lineNum);
    lineNumberLabel->setPosition({ m_lineNumberWidth, theme.lineHeight / 2 });
    codeLabel->setScale(theme.fontSize);
    codeLabel->setAnchorPoint(BOTTOM_LEFT);
    codeLabel->setPosition({ m_lineNumberWidth + PADDING, theme.lineHeight / 2 });
    this->addChild(lineNumberLabel);
    this->addChild(codeLabel);

    return true;
}