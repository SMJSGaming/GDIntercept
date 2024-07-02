#include "CodeBlock.hpp"

std::vector<std::pair<char, SEL_MenuHandler>> CodeBlock::dataTypes({
    { 'B', menu_selector(CodeBlock::onBody) },
    { 'Q', menu_selector(CodeBlock::onQuery) },
    { 'H', menu_selector(CodeBlock::onHeaders) },
    { 'R', menu_selector(CodeBlock::onResponse) }
});

size_t CodeBlock::buttonCount = CodeBlock::dataTypes.size();

char CodeBlock::currentDataType = CodeBlock::dataTypes.at(0).first;

CodeBlock* CodeBlock::create(const CCSize& size, const CCSize& buttonBarSize) {
    CodeBlock* instance = new CodeBlock();

    if (instance && instance->init(size, buttonBarSize)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CodeBlock::init(const CCSize& size, const CCSize& buttonBarSize) {
    const size_t buttonCount = CodeBlock::dataTypes.size();
    std::vector<CCMenuItemSpriteExtra*> buttons;

    if (!JSONCodeBlock::init({ HttpInfo::UNKNOWN_CONTENT, "" }, size)) {
        return false;
    }

    for (size_t i = 0; i < buttonCount; i++) {
        const auto& [key, selector] = CodeBlock::dataTypes.at(i);
        std::string keyStr(1, key);
        CCLabelBMFont* label = CCLabelBMFont::create(keyStr.c_str(), "consola.fnt"_spr);

        buttons.push_back(CCMenuItemSpriteExtra::create(label, this, selector));
        label->setScale(0.8f);
        label->setAnchorPoint(CENTER);
        label->setColor(ThemeStyle::getTheme().lineNum);
        label->setPosition(ccp(buttonBarSize.width / buttonCount, buttonBarSize.height) / 2);
        m_labels.insert({ key, label });

        cocos::getChild<CCSprite>(label, 0)->setPositionY(buttonBarSize.height / 2);
    }

    ButtonBar* buttonBar = ButtonBar::create("square02_001.png", 0.2f, buttonBarSize, buttons);

    buttonBar->setPosition({ size.width - PADDING * 1.5f, size.height - PADDING });
    buttonBar->setAnchorPoint(TOP_RIGHT);
    this->addChild(buttonBar);

    return true;
}

void CodeBlock::onBody(CCObject* sender) {
    this->setCode(m_request->getBodyContent(Mod::get()->getSettingValue<bool>("raw-data")));
    this->updateDataTypeColor('B');
}

void CodeBlock::onQuery(CCObject* sender) {
    this->setCode({ HttpInfo::JSON, m_request->stringifyQuery() });
    this->updateDataTypeColor('Q');
}

void CodeBlock::onHeaders(CCObject* sender) {
    this->setCode({ HttpInfo::JSON, m_request->stringifyQuery() });
    this->updateDataTypeColor('H');
}

void CodeBlock::onResponse(CCObject* sender) {
    this->setCode(m_request->getResponseContent(Mod::get()->getSettingValue<bool>("raw-data")));
    this->updateDataTypeColor('R');
}

void CodeBlock::updateDataTypeColor(char type) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    currentDataType = type;

    for (const auto& [key, _] : CodeBlock::dataTypes) {
        m_labels.at(key)->setColor(key == type ? theme.text : theme.lineNum);
    }
}

void CodeBlock::updateRequest(HttpInfo* request) {
    m_request = request;

    switch (currentDataType) {
        case 'B': this->onBody(nullptr); break;
        case 'Q': this->onQuery(nullptr); break;
        case 'H': this->onHeaders(nullptr); break;
        case 'R': this->onResponse(nullptr); break;
    }
}