#include "InterceptPopup.hpp"

CCSize InterceptPopup::popupPadding = { 60, 40 };

float InterceptPopup::uiPadding = 2 + PADDING;

float InterceptPopup::captureCellHeight = 20;

float InterceptPopup::captureCellBadgeHeight = 10;

InterceptPopup* InterceptPopup::get() {
    return as<InterceptPopup*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("intercept_popup"_spr));
}

void InterceptPopup::scene() {
    const CCSize uiSize = CCDirector::sharedDirector()->getWinSize() - InterceptPopup::popupPadding;

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
m_leftColumnXPosition(InterceptPopup::uiPadding + PADDING + m_captureCellWidth),
m_codeBlock(nullptr),
m_list(nullptr) { }

bool InterceptPopup::setup() {
    this->setTitle("Intercepted Requests");
    this->setupCodeBlock();
    this->setupList();

    CCSprite* settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(InterceptPopup::onSettings));

    settingsSprite->setScale(0.4f);
    item->setPosition({
        m_size.width - InterceptPopup::uiPadding - settingsSprite->getContentWidth() * settingsSprite->getScale() / 2,
        m_title->getPositionY()
    });
    m_buttonMenu->addChild(item);

    return true;
}

void InterceptPopup::reloadList() {
    OPT(m_list)->removeFromParentAndCleanup(true);

    this->setupList();

    if (SETTINGS_POPUP) {
        SETTINGS_POPUP->removeFromParentAndCleanup(true);

        this->onSettings(nullptr);
    }
}

void InterceptPopup::reloadCodeBlock(const bool recycleInfo) {
    OPT(m_codeBlock)->removeFromParentAndCleanup(true);

    this->setupCodeBlock(recycleInfo);

    if (SETTINGS_POPUP) {
        SETTINGS_POPUP->removeFromParentAndCleanup(true);

        this->onSettings(nullptr);
    }
}

void InterceptPopup::reloadCode() {
    m_codeBlock->reloadCode();
}

void InterceptPopup::reloadSideBar() {
    m_codeBlock->reloadSideBar();
}

void InterceptPopup::setupList() {
    const float cellHeight = InterceptPopup::captureCellHeight + InterceptPopup::captureCellBadgeHeight * !Mod::get()->getSettingValue<bool>("hide-badges");

    m_list = CaptureList::create({
        m_captureCellWidth,
        this->getPageHeight()
    }, cellHeight, [this](HttpInfo* info) {
        m_codeBlock->updateInfo(info);
    });

    m_list->setAnchorPoint(BOTTOM_LEFT);
    m_list->setPosition({ InterceptPopup::uiPadding, InterceptPopup::uiPadding });
    m_mainLayer->addChild(m_list);
}

void InterceptPopup::setupCodeBlock(const bool recycleInfo) {
    HttpInfo* info = recycleInfo ? m_codeBlock->getActiveInfo() : nullptr;

    m_codeBlock = CodeBlock::create({
        m_size.width - m_leftColumnXPosition - InterceptPopup::uiPadding,
        this->getPageHeight()
    });

    if (recycleInfo) {
        m_codeBlock->updateInfo(info);
    }

    m_codeBlock->setPosition({ m_leftColumnXPosition, InterceptPopup::uiPadding });
    m_mainLayer->addChild(m_codeBlock);
}

float InterceptPopup::getPageHeight() {
    return m_title->getPositionY() - m_title->getContentHeight() / 2 - InterceptPopup::uiPadding;
}

float InterceptPopup::getComponentYPosition(float offset, float itemHeight) {
    return InterceptPopup::uiPadding + this->getPageHeight() - itemHeight - offset;
}

void InterceptPopup::onSettings(CCObject* obj) {
    CCScene* currentScene = CCDirector::sharedDirector()->getRunningScene();

    openSettingsPopup(Mod::get());

    // If this breaks, someone should stop messing with addChild, that's a whole lot of not my problem
    SETTINGS_POPUP = as<FLAlertLayer*>(currentScene->getChildren()->objectAtIndex(currentScene->getChildrenCount() - 1));
}