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

InterceptPopup* InterceptPopup::get() {
    return as<InterceptPopup*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("intercept_popup"_spr));
}

void InterceptPopup::scene() {
    if (!ProxyHandler::getProxies().empty() && !InterceptPopup::get()) {
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
    this->reload();

    CCSprite* settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");

    settingsSprite->setScale(0.4f);

    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(settingsSprite, this, menu_selector(InterceptPopup::onSettings));

    item->setPosition({
        InterceptPopup::uiWidth - InterceptPopup::uiPadding - settingsSprite->getContentWidth() * settingsSprite->getScale() / 2,
        m_title->getPositionY()
    });
    m_buttonMenu->addChild(item);

    return true;
}

void InterceptPopup::reload() {
    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
    const size_t sceneNodeCount = scene->getChildrenCount();

    OPT(m_infoArea)->removeFromParentAndCleanup(true);
    OPT(m_controls)->removeFromParentAndCleanup(true);
    OPT(m_codeBlock)->removeFromParentAndCleanup(true);
    OPT(m_list)->removeFromParentAndCleanup(true);

    m_infoArea = this->setupInfo();
    m_controls = this->setupControls();
    m_codeBlock = this->setupCodeBlock();
    m_list = this->setupList();

    if (m_settings) {
        m_settings->removeFromParentAndCleanup(true);
        this->onSettings(nullptr);
    }
}

void InterceptPopup::copyCode() {
    m_codeBlock->copyCode();
}

InfoArea* InterceptPopup::setupInfo() {
    InfoArea* infoArea = InfoArea::create({ InterceptPopup::infoWidth, InterceptPopup::infoRowHeight });

    m_mainLayer->addChild(infoArea);
    infoArea->setPosition({
        InterceptPopup::middleColumnXPosition,
        this->getComponentYPosition(0, InterceptPopup::infoRowHeight)
    });

    return infoArea;
}

Border* InterceptPopup::setupList() {
    const CCSize listSize(ccp(InterceptPopup::captureCellWidth, this->getPageHeight()));
    Border* captures = Border::create(CaptureList::create(listSize - 2, InterceptPopup::captureCellHeight, [this](HttpInfo* request) {
        m_infoArea->updateRequest(request);
        m_codeBlock->updateRequest(request);
    }), LIGHTER_BROWN_4B, listSize, { 1, 1 });

    captures->setAnchorPoint(BOTTOM_LEFT);
    captures->setPosition({ InterceptPopup::uiPadding, InterceptPopup::uiPadding });
    m_mainLayer->addChild(captures);

    return captures;
}

ControlMenu* InterceptPopup::setupControls() {
    const float xPosition = m_infoArea->getPositionX() + m_infoArea->getContentWidth() + PADDING;

    ControlMenu* controls = ControlMenu::create({
        InterceptPopup::uiWidth - xPosition - InterceptPopup::uiPadding,
        InterceptPopup::infoRowHeight
    }, this->m_settings);

    controls->setPosition({ xPosition, this->getComponentYPosition(0, InterceptPopup::infoRowHeight) });
    m_mainLayer->addChild(controls);

    return controls;
}

CodeBlock* InterceptPopup::setupCodeBlock() {
    CodeBlock* codeBlock = CodeBlock::create({
        InterceptPopup::uiWidth - InterceptPopup::middleColumnXPosition - InterceptPopup::uiPadding,
        this->getPageHeight() - InterceptPopup::infoRowHeight - PADDING
    }, {
        CodeBlock::buttonCount * InterceptPopup::codeBlockButtonHeight,
        InterceptPopup::codeBlockButtonHeight
    });

    codeBlock->setPosition({ InterceptPopup::middleColumnXPosition, InterceptPopup::uiPadding });
    m_mainLayer->addChild(codeBlock);

    return codeBlock;
}

float InterceptPopup::getPageHeight() {
    return m_title->getPositionY() - m_title->getContentHeight() / 2 - InterceptPopup::uiPadding;
}

float InterceptPopup::getComponentYPosition(float offset, float itemHeight) {
    return InterceptPopup::uiPadding + this->getPageHeight() - itemHeight - offset;
}

void InterceptPopup::onClose(CCObject* obj) {
    this->removeFromParent();
}

void InterceptPopup::onSettings(CCObject* obj) {
    CCScene* currentScene = CCDirector::sharedDirector()->getRunningScene();

    openSettingsPopup(Mod::get());

    // If this breaks, someone should stop messing with addChild, that's a whole lot of not my problem
    m_settings = as<FLAlertLayer*>(currentScene->getChildren()->objectAtIndex(currentScene->getChildrenCount() - 1));
}