#include "InterceptPopup.hpp"

float InterceptPopup::uiWidth = 500;

float InterceptPopup::uiHeight = 280;

float InterceptPopup::uiPadding = 2 + PADDING;

float InterceptPopup::captureCellWidth = 170;

float InterceptPopup::captureCellHeight = 20;

float InterceptPopup::infoWidth = 160;

float InterceptPopup::infoRowHeight = 65;

float InterceptPopup::codeBlockButtonHeight = 15;

float InterceptPopup::middleColumnXPosition = InterceptPopup::uiPadding + PADDING + InterceptPopup::captureCellWidth;

std::vector<std::pair<char, SEL_MenuHandler>> InterceptPopup::dataTypes({
    { 'B', menu_selector(InterceptPopup::onBody) },
    { 'Q', menu_selector(InterceptPopup::onQuery) },
    { 'H', menu_selector(InterceptPopup::onHeaders) },
    { 'R', menu_selector(InterceptPopup::onResponse) }
});

char InterceptPopup::currentDataType = InterceptPopup::dataTypes.at(0).first;

float InterceptPopup::codeBlockButtonWidth = InterceptPopup::dataTypes.size() * InterceptPopup::codeBlockButtonHeight;

std::vector<std::tuple<std::string, std::string, SEL_MenuHandler>> InterceptPopup::toggles({
    { "Pause requests", "pause-requests", menu_selector(InterceptPopup::onPauseRequest) },
    { "Censor sensitive data", "censor-data", menu_selector(InterceptPopup::onCensorData) },
    { "Remember requests", "remember-requests", menu_selector(InterceptPopup::onRememberRequests) },
    { "Raw Data", "raw-data", menu_selector(InterceptPopup::onRawData) }
});

InterceptPopup* InterceptPopup::get() {
    return as<InterceptPopup*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("intercept_popup"_spr));
}

void InterceptPopup::scene() {
    if (!HttpInfo::requests.empty() && !InterceptPopup::get()) {
        InterceptPopup* instance = new InterceptPopup();

        if (instance && instance->initAnchored(InterceptPopup::uiWidth, InterceptPopup::uiHeight)) {
            instance->m_noElasticity = true;
            instance->setID("intercept_popup"_spr);
            instance->retain();
            instance->show();
        } else {
            CC_SAFE_DELETE(instance);
        }
    }
}

bool InterceptPopup::setup() {
    this->setTitle("Intercepted Requests");
    this->setupCodeBlock();
    this->setupInfo();
    this->setupSettings();
    this->setupList();

    return true;
}

void InterceptPopup::onClose(CCObject* obj) {
    this->removeFromParent();
}

void InterceptPopup::setupList() {
    const CCSize listSize(ccp(InterceptPopup::captureCellWidth, this->getPageHeight()));
    BorderFix* captures = BorderFix::create(m_captureList = CaptureList::create(listSize - 2, InterceptPopup::captureCellHeight, [this](HttpInfo* request) {
        m_currentRequest = request;

        this->updateInfo();
    }), LIGHTER_BROWN_4B, listSize, { 1, 1 });

    captures->setAnchorPoint(BOTTOM_LEFT);
    captures->setPosition({ InterceptPopup::uiPadding, InterceptPopup::uiPadding });
    m_mainLayer->addChild(captures);
}

void InterceptPopup::setupSettings() {
    CCNode* info = m_mainLayer->getChildByID("info"_spr);
    CCSize padding(ccp(PADDING, PADDING));
    const float xPosition = info->getPositionX() + info->getContentWidth() + PADDING;
    const float width = InterceptPopup::uiWidth - xPosition - InterceptPopup::uiPadding;
    CCScale9Sprite* settingsBg = CCScale9Sprite::create("square02b_001.png");
    BorderFix* settings = BorderFix::create(settingsBg, LIGHTER_BROWN_4B, { width, InterceptPopup::infoRowHeight }, padding);
    CCMenu* toggleMenu = CCMenu::create();
    Mod* mod = Mod::get();

    toggleMenu->setPosition(ZERO_POINT);
    toggleMenu->setContentSize(settingsBg->getContentSize() - padding * 2);

    for (int i = 0; i < InterceptPopup::toggles.size(); i++) {
        const auto& [name, key, selector] = InterceptPopup::toggles.at(i);

        GameToolbox::createToggleButton(name, selector, mod->getSettingValue<bool>(key), toggleMenu, {
            PADDING,
            toggleMenu->getContentHeight() - PADDING * (i * 2.4f + 1)
        }, this, toggleMenu, 0.3f, 0.3f, width - PADDING * 2, { 2, 0 }, "bigFont.fnt", false, 0, nullptr)->setID(""_spr + key);
    }

    toggleMenu->setPosition(padding);
    settings->setPosition({ xPosition, this->getComponentYPosition(0, InterceptPopup::infoRowHeight) });
    settingsBg->setColor(LIGHT_BROWN_3B);
    settingsBg->addChild(toggleMenu);
    m_mainLayer->addChild(settings);
}

void InterceptPopup::setupInfo() {
    const float xPosition = InterceptPopup::uiPadding + PADDING + InterceptPopup::captureCellWidth;
    SimpleTextArea* infoText = SimpleTextArea::create("", "chatFont.fnt", 0.5f, InterceptPopup::infoWidth - 20);
    CCScale9Sprite* infoBg = CCScale9Sprite::create("square02b_001.png");
    BorderFix* info = BorderFix::create(infoBg, LIGHTER_BROWN_4B, {
        InterceptPopup::infoWidth,
        InterceptPopup::infoRowHeight
    }, { PADDING, PADDING });

    info->setID("info"_spr);
    info->setPosition({ xPosition, this->getComponentYPosition(0, InterceptPopup::infoRowHeight) });
    infoBg->addChild(infoText);
    infoBg->setColor(LIGHT_BROWN_3B);
    infoText->setID("info_text"_spr);
    infoText->setAnchorPoint(CENTER_LEFT);
    infoText->setPosition({ 5, infoBg->getContentHeight() / 2 });
    infoText->setMaxLines(5);
    infoText->setLinePadding(2);
    infoText->setWrappingMode(NO_WRAP);

    m_mainLayer->addChild(info);
}

void InterceptPopup::setupCodeBlock() {
    const float width = InterceptPopup::uiWidth - InterceptPopup::middleColumnXPosition - InterceptPopup::uiPadding;
    const float codeBlockHeight = this->getPageHeight() - InterceptPopup::infoRowHeight - PADDING;
    const size_t buttonCount = InterceptPopup::dataTypes.size();
    JSONCodeBlock* codeBlock = JSONCodeBlock::create({ HttpInfo::UNKNOWN_CONTENT, "" }, { width, codeBlockHeight });
    std::vector<CCMenuItemSpriteExtra*> buttons;

    for (size_t i = 0; i < buttonCount; i++) {
        const float buttonWidth = width / 3;
        const auto& [key, selector] = InterceptPopup::dataTypes.at(i);
        std::string keyStr(1, key);
        CCLabelBMFont* label = CCLabelBMFont::create(keyStr.c_str(), "consola.fnt"_spr);
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(label, this, selector);

        label->setID(""_spr + keyStr);
        label->setScale(0.8f);
        label->setAnchorPoint(CENTER);
        label->setColor(ThemeStyle::getTheme().line);
        label->setPosition(ccp(InterceptPopup::codeBlockButtonWidth / buttonCount, InterceptPopup::codeBlockButtonHeight) / 2);
        button->setSizeMult(0);
        buttons.push_back(button);

        cocos::getChild<CCSprite>(label, 0)->setPositionY(InterceptPopup::codeBlockButtonHeight / 2);
    }

    ButtonBar* buttonBar = ButtonBar::create("square02_001.png", 0.2f, {
        InterceptPopup::codeBlockButtonWidth,
        InterceptPopup::codeBlockButtonHeight
    }, buttons);

    codeBlock->setID("info_code"_spr);
    codeBlock->setPosition({ InterceptPopup::middleColumnXPosition, InterceptPopup::uiPadding });
    buttonBar->setID("code_buttons"_spr);
    buttonBar->setPosition({ InterceptPopup::middleColumnXPosition + width - PADDING * 1.5f, codeBlockHeight + InterceptPopup::uiPadding - PADDING });
    buttonBar->setAnchorPoint(TOP_RIGHT);
    m_mainLayer->addChild(codeBlock);
    m_mainLayer->addChild(buttonBar);
}

void InterceptPopup::updateInfo() {
    as<SimpleTextArea*>(m_mainLayer->getChildByIDRecursive("info_text"_spr))->setText(m_currentRequest->generateBasicInfo());

    switch (currentDataType) {
        case 'B': this->onBody(nullptr); break;
        case 'Q': this->onQuery(nullptr); break;
        case 'H': this->onHeaders(nullptr); break;
        case 'R': this->onResponse(nullptr); break;
    }
}

float InterceptPopup::getPageHeight() {
    return m_title->getPositionY() - m_title->getContentHeight() / 2 - InterceptPopup::uiPadding;
}

float InterceptPopup::getComponentYPosition(float offset, float itemHeight) {
    return InterceptPopup::uiPadding + this->getPageHeight() - itemHeight - offset;
}

void InterceptPopup::updateDataTypeColor(char type) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    currentDataType = type;

    for (const auto& [key, _] : InterceptPopup::dataTypes) {
        as<CCLabelBMFont*>(
            m_mainLayer->getChildByID("code_buttons"_spr)->getChildByIDRecursive(""_spr + std::string(1, key))
        )->setColor(key == type ? theme.text : theme.line);
    }
}

void InterceptPopup::onPauseRequest(CCObject* sender) {
    Mod::get()->setSettingValue("pause-requests", !as<CCMenuItemToggler*>(sender)->isOn());
}

void InterceptPopup::onCensorData(CCObject* sender) {
    Mod::get()->setSettingValue("censor-data", !as<CCMenuItemToggler*>(sender)->isOn());

    this->updateInfo();
}

void InterceptPopup::onRememberRequests(CCObject* sender) {
    const bool value = !as<CCMenuItemToggler*>(sender)->isOn();

    Mod::get()->setSettingValue("remember-requests", value);

    if (!value) {
        m_captureList->createCells();
    }
}

void InterceptPopup::onRawData(CCObject* sender) {
    Mod::get()->setSettingValue("raw-data", !as<CCMenuItemToggler*>(sender)->isOn());

    this->updateInfo();
}

void InterceptPopup::onBody(CCObject* sender) {
    as<JSONCodeBlock*>(m_mainLayer->getChildByID("info_code"_spr))->setCode(m_currentRequest->formatBody());
    this->updateDataTypeColor('B');
}

void InterceptPopup::onQuery(CCObject* sender) {
    as<JSONCodeBlock*>(m_mainLayer->getChildByID("info_code"_spr))->setCode({ HttpInfo::JSON, m_currentRequest->formatQuery() });
    this->updateDataTypeColor('Q');
}

void InterceptPopup::onHeaders(CCObject* sender) {
    as<JSONCodeBlock*>(m_mainLayer->getChildByID("info_code"_spr))->setCode({ HttpInfo::JSON, m_currentRequest->formatHeaders() });
    this->updateDataTypeColor('H');
}

void InterceptPopup::onResponse(CCObject* sender) {
    as<JSONCodeBlock*>(m_mainLayer->getChildByID("info_code"_spr))->setCode(m_currentRequest->formatResponse());
    this->updateDataTypeColor('R');
}