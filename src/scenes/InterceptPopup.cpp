#include "InterceptPopup.hpp"

InterceptPopup* InterceptPopup::instance = nullptr;

float InterceptPopup::uiWidth = 500;

float InterceptPopup::uiHeight = 280;

float InterceptPopup::uiPadding = (CCDirector::sharedDirector()->getWinSize().width - InterceptPopup::uiWidth) / 2 + 2;

float InterceptPopup::captureCellWidth = 170;

float InterceptPopup::captureCellHeight = 20;

float InterceptPopup::infoWidth = 160;

float InterceptPopup::infoRowHeight = 65;

float InterceptPopup::codeBlockButtonHeight = 15;

float InterceptPopup::middleColumnXPosition = InterceptPopup::uiPadding + PADDING * 2 + InterceptPopup::captureCellWidth;

std::vector<std::tuple<char, SEL_MenuHandler>> InterceptPopup::dataTypes({
    { 'B', menu_selector(InterceptPopup::onBody) },
    { 'Q', menu_selector(InterceptPopup::onQuery) },
    { 'H', menu_selector(InterceptPopup::onHeaders) },
    { 'R', menu_selector(InterceptPopup::onResponse) }
});

float InterceptPopup::codeBlockButtonWidth = dataTypes.size() * InterceptPopup::codeBlockButtonHeight;

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
        CCScene* currentScene = CCDirector::sharedDirector()->getRunningScene();

        if (instance) {
            Mod* mod = Mod::get();

            for (const auto& [_1, key, _2] : InterceptPopup::toggles) {
                as<CCMenuItemToggler*>(instance->getChildByIDRecursive(""_spr + key))->toggle(mod->getSettingValue<bool>(key));
            }

            instance->m_captureList->createCells();
            instance->updateInfo(instance->m_currentRequest);
        } else {
            instance = new InterceptPopup();

            if (instance && instance->init(InterceptPopup::uiWidth, InterceptPopup::uiHeight)) {
                instance->setID("intercept_popup"_spr);
                instance->retain();
            } else {
                CC_SAFE_DELETE(instance);

                return;
            }
        }

        instance->setTouchPriority(100);
        currentScene->addChild(instance, currentScene->getHighestChildZ() + 1);
    }
}

bool InterceptPopup::setup() {
    m_currentDataType = std::get<0>(InterceptPopup::dataTypes.at(0));
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
    const float xPosition = InterceptPopup::uiPadding + PADDING;
    const CCSize listSize({ InterceptPopup::captureCellWidth, this->getPageHeight() });
    BorderFix* captures = BorderFix::create(m_captureList = CaptureList::create(listSize - 2, InterceptPopup::captureCellHeight, [this](HttpInfo* request) {
        this->updateInfo(request);
    }), LIGHTER_BROWN_4B, listSize, { 1, 1 });

    captures->setAnchorPoint(BOTTOM_LEFT);
    captures->setPosition({ xPosition, this->getYPadding() });
    m_mainLayer->addChild(captures);
}

void InterceptPopup::setupSettings() {
    CCNode* info = m_mainLayer->getChildByID("info"_spr);
    CCSize padding({ PADDING, PADDING });
    const float xPosition = info->getPositionX() + info->getContentWidth() + PADDING;
    const float width = CCDirector::sharedDirector()->getWinSize().width - xPosition - InterceptPopup::uiPadding - PADDING;
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
    const float xPosition = InterceptPopup::uiPadding + PADDING * 2 + InterceptPopup::captureCellWidth;
    SimpleTextArea* infoText = SimpleTextArea::create("", "chatFont.fnt", 0.5f, InterceptPopup::infoWidth - 20);
    CCScale9Sprite* infoBg = CCScale9Sprite::create("square02b_001.png");
    BorderFix* info = BorderFix::create(infoBg, LIGHTER_BROWN_4B, { InterceptPopup::infoWidth, InterceptPopup::infoRowHeight }, { PADDING, PADDING });

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
    const float width = CCDirector::sharedDirector()->getWinSize().width - InterceptPopup::middleColumnXPosition - PADDING - InterceptPopup::uiPadding;
    const float codeBlockHeight = this->getPageHeight() - InterceptPopup::infoRowHeight - PADDING;
    const float yPadding = this->getYPadding();
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
        label->setPosition(CCSize({ InterceptPopup::codeBlockButtonWidth / buttonCount, InterceptPopup::codeBlockButtonHeight }) / 2);
        button->setSizeMult(0);
        buttons.push_back(button);

        cocos::getChild<CCSprite>(label, 0)->setPositionY(InterceptPopup::codeBlockButtonHeight / 2);
    }

    ButtonBar* buttonBar = ButtonBar::create("square02_001.png", 0.2f, { InterceptPopup::codeBlockButtonWidth, InterceptPopup::codeBlockButtonHeight }, buttons);

    codeBlock->setID("info_code"_spr);
    codeBlock->setPosition({ InterceptPopup::middleColumnXPosition, yPadding });
    buttonBar->setID("code_buttons"_spr);
    buttonBar->setPosition({ InterceptPopup::middleColumnXPosition + width - PADDING * 1.5f, codeBlockHeight + yPadding - PADDING });
    buttonBar->setAnchorPoint(TOP_RIGHT);
    m_mainLayer->addChild(codeBlock);
    m_mainLayer->addChild(buttonBar);
}

void InterceptPopup::updateInfo(HttpInfo* request) {
    m_currentRequest = request;

    as<SimpleTextArea*>(m_mainLayer->getChildByIDRecursive("info_text"_spr))->setText(request->generateBasicInfo());

    switch (m_currentDataType) {
        case 'B': this->onBody(nullptr); break;
        case 'Q': this->onQuery(nullptr); break;
        case 'H': this->onHeaders(nullptr); break;
        case 'R': this->onResponse(nullptr); break;
    }
}

float InterceptPopup::getPageHeight() {
    return m_title->getPositionY() - m_title->getContentHeight() * 1.5f - PADDING;
}

float InterceptPopup::getYPadding() {
    return m_title->getContentHeight() + PADDING;
}

float InterceptPopup::getComponentYPosition(float offset, float itemHeight) {
    return this->getYPadding() + this->getPageHeight() - itemHeight - offset;
}

void InterceptPopup::updateDataTypeColor(char type) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    m_currentDataType = type;

    for (const auto& [key, _] : InterceptPopup::dataTypes) {
        as<CCLabelBMFont*>(m_mainLayer->getChildByID("code_buttons"_spr)->getChildByIDRecursive(""_spr + std::string(1, key)))->setColor(key == type ? theme.text : theme.line);
    }
}

void InterceptPopup::onPauseRequest(CCObject* sender) {
    Mod::get()->setSettingValue("pause-requests", !as<CCMenuItemToggler*>(sender)->isOn());
}

void InterceptPopup::onCensorData(CCObject* sender) {
    Mod::get()->setSettingValue("censor-data", !as<CCMenuItemToggler*>(sender)->isOn());

    this->updateInfo(m_currentRequest);
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

    this->updateInfo(m_currentRequest);
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