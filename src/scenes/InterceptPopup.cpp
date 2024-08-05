#include "InterceptPopup.hpp"

CCSize InterceptPopup::popupPadding = { 60, 40 };

float InterceptPopup::uiPadding = 2 + PADDING;

float InterceptPopup::captureCellHeight = 20;

float InterceptPopup::captureCellBadgeHeight = 10;

float InterceptPopup::infoRowHeight = 65;

float InterceptPopup::codeBlockButtonHeight = 15;

InterceptPopup* InterceptPopup::get() {
    return as<InterceptPopup*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("intercept_popup"_spr));
}

void InterceptPopup::scene() {
    CCSize uiSize = CCDirector::sharedDirector()->getWinSize() - InterceptPopup::popupPadding;

    if (!InterceptPopup::get()) {
        InterceptPopup* instance = new InterceptPopup(uiSize);

        if (instance && instance->initAnchored(uiSize.width, uiSize.height)) {
            instance->m_noElasticity = true;
            instance->setID("intercept_popup"_spr);
            instance->autorelease();
            instance->show();
        } else {
            CC_SAFE_DELETE(instance);
        }
    }
}

InterceptPopup::InterceptPopup(const CCSize& size) : m_captureCellWidth(size.width / 3),
m_infoWidth(size.width / 3),
m_middleColumnXPosition(InterceptPopup::uiPadding + PADDING + m_captureCellWidth),
m_infoArea(nullptr),
m_controls(nullptr),
m_codeBlock(nullptr),
m_list(nullptr),
m_settings(nullptr) { }

bool InterceptPopup::setup() {
    this->setTitle("Intercepted Requests");
    this->reload();

    CCSprite* settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");

    settingsSprite->setScale(0.4f);

    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(InterceptPopup::onSettings));

    item->setPosition({
        m_size.width - InterceptPopup::uiPadding - settingsSprite->getContentWidth() * settingsSprite->getScale() / 2,
        m_title->getPositionY()
    });
    m_buttonMenu->addChild(item);

    return true;
}

void InterceptPopup::softReload() {
    OPT(m_infoArea)->removeFromParentAndCleanup(true);
    OPT(m_list)->removeFromParentAndCleanup(true);

    m_infoArea = this->setupInfo();
    m_list = this->setupList();

    this->postReload();
}

void InterceptPopup::reload() {
    OPT(m_infoArea)->removeFromParentAndCleanup(true);
    OPT(m_controls)->removeFromParentAndCleanup(true);
    OPT(m_codeBlock)->removeFromParentAndCleanup(true);
    OPT(m_list)->removeFromParentAndCleanup(true);

    m_infoArea = this->setupInfo();
    m_controls = this->setupControls();
    m_codeBlock = this->setupCodeBlock();
    m_list = this->setupList();

    this->postReload();
}

InfoArea* InterceptPopup::setupInfo() {
    InfoArea* infoArea = InfoArea::create({ m_infoWidth, InterceptPopup::infoRowHeight });

    m_mainLayer->addChild(infoArea);
    infoArea->setPosition({
        m_middleColumnXPosition,
        this->getComponentYPosition(0, InterceptPopup::infoRowHeight)
    });

    return infoArea;
}

CaptureList* InterceptPopup::setupList() {
    CaptureList* list = CaptureList::create({
        m_captureCellWidth,
        this->getPageHeight()
    }, InterceptPopup::captureCellHeight + InterceptPopup::captureCellBadgeHeight * !Mod::get()->getSettingValue<bool>("hide-badges"), [
        this
    ](const HttpInfo* info) {
        m_infoArea->updateInfo(info);
        m_controls->updateInfo(info);
        m_codeBlock->updateInfo(info);
    });

    list->setAnchorPoint(BOTTOM_LEFT);
    list->setPosition({ InterceptPopup::uiPadding, InterceptPopup::uiPadding });
    m_mainLayer->addChild(list);

    return list;
}

ControlMenu* InterceptPopup::setupControls() {
    const float xPosition = m_infoArea->getPositionX() + m_infoArea->getContentWidth() + PADDING;

    ControlMenu* controls = ControlMenu::create({
        m_size.width - xPosition - InterceptPopup::uiPadding,
        InterceptPopup::infoRowHeight
    }, m_settings);

    controls->setPosition({ xPosition, this->getComponentYPosition(0, InterceptPopup::infoRowHeight) });
    m_mainLayer->addChild(controls);

    return controls;
}

CodeBlock* InterceptPopup::setupCodeBlock() {
    CodeBlock* codeBlock = CodeBlock::create({
        m_size.width - m_middleColumnXPosition - InterceptPopup::uiPadding,
        this->getPageHeight() - InterceptPopup::infoRowHeight - PADDING
    }, {
        CodeBlock::buttonCount * InterceptPopup::codeBlockButtonHeight,
        InterceptPopup::codeBlockButtonHeight
    });

    codeBlock->setPosition({ m_middleColumnXPosition, InterceptPopup::uiPadding });
    m_mainLayer->addChild(codeBlock);

    return codeBlock;
}

float InterceptPopup::getPageHeight() {
    return m_title->getPositionY() - m_title->getContentHeight() / 2 - InterceptPopup::uiPadding;
}

float InterceptPopup::getComponentYPosition(float offset, float itemHeight) {
    return InterceptPopup::uiPadding + this->getPageHeight() - itemHeight - offset;
}

void InterceptPopup::postReload() {
    if (m_settings) {
        m_settings->removeFromParentAndCleanup(true);
        this->onSettings(nullptr);
    }
}

void InterceptPopup::onClose(CCObject* obj) {
    this->removeFromParentAndCleanup(true);
}

void InterceptPopup::onSettings(CCObject* obj) {
    CCScene* currentScene = CCDirector::sharedDirector()->getRunningScene();

    openSettingsPopup(Mod::get());

    // If this breaks, someone should stop messing with addChild, that's a whole lot of not my problem
    m_settings = as<FLAlertLayer*>(currentScene->getChildren()->objectAtIndex(currentScene->getChildrenCount() - 1));
}