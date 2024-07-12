#include "CodeLineCell.hpp"

CodeLineCell* CodeLineCell::create(const HttpInfo::HttpContent& code, const size_t lineNumber, const float lineNumberWidth, JSONColor& color) {
    CodeLineCell* instance = new CodeLineCell();

    if (instance && instance->init(code, lineNumber, lineNumberWidth, color)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CodeLineCell::init(const HttpInfo::HttpContent& code, const size_t lineNumber, const float lineNumberWidth, JSONColor& color) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    // Fuck it, we're truncating this bitch
    std::string truncatedCode = code.contents.size() >= 300 ? code.contents.substr(0, 300) : std::string(code.contents);
    CCLabelBMFont* lineNumberLabel = CCLabelBMFont::create(std::to_string(lineNumber).c_str(), theme.fontName);

    if (!CCLayer::init()) {
        return false;
    }

    for (size_t index = truncatedCode.find('\t'); index != std::string::npos; index = truncatedCode.find('\t')) {
        truncatedCode.replace(index, 1, "  ");
    }

    CCLabelBMFont* codeLabel = CCLabelBMFont::create(truncatedCode.c_str(), theme.fontName);

    if (code.type == HttpInfo::JSON) {
        color.parseLine(code.contents, codeLabel);
    } else {
        codeLabel->setColor(theme.text);
    }

    lineNumberLabel->setScale(theme.fontSize);
    lineNumberLabel->setAnchorPoint(BOTTOM_RIGHT);
    lineNumberLabel->setColor(theme.lineNum);
    lineNumberLabel->setPosition({ lineNumberWidth, theme.lineHeight / 2 });
    codeLabel->setScale(theme.fontSize);
    codeLabel->setAnchorPoint(BOTTOM_LEFT);
    codeLabel->setPosition({ lineNumberWidth + PADDING, theme.lineHeight / 2 });
    this->addChild(lineNumberLabel);
    this->addChild(codeLabel);

    return true;
}