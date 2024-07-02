#include "ControlMenu.hpp"

ControlMenu* ControlMenu::create(const CCSize& size, FLAlertLayer*& settings) {
    ControlMenu* instance = new ControlMenu(settings);

    if (instance && instance->init(size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

ControlMenu::ControlMenu(FLAlertLayer*& settings): m_settings(settings) { }

bool ControlMenu::init(const CCSize& size) {
    const CCSize menuSize(size - PADDING * 2);
    const CCSize buttonSize(ccp(menuSize.width - PADDING * 6, (menuSize.height - PADDING * 3) / 2));
    CCScale9Sprite* infoBg = CCScale9Sprite::create("square02b_001.png");
    m_pauseButton = ButtonSprite::create(
        Mod::get()->getSettingValue<bool>("pause-requests") ? "Resume" : "Pause",
        buttonSize.width,
        true,
        "bigFont.fnt",
        "GJ_button_01.png",
        buttonSize.height,
        0.5f
    );
    m_sendButton = ButtonSprite::create(
        "Send",
        buttonSize.width,
        true,
        "bigFont.fnt",
        "GJ_button_01.png",
        buttonSize.height,
        0.5f
    );
    CCMenuItemSpriteExtra* pauseItem = CCMenuItemSpriteExtra::create(m_pauseButton, this, menu_selector(ControlMenu::onPause));
    CCMenuItemSpriteExtra* sendItem = CCMenuItemSpriteExtra::create(m_sendButton, this, menu_selector(ControlMenu::onSend));
    CCMenu* menu = CCMenu::create();

    if (!Border::init(infoBg, LIGHTER_BROWN_4B, size)) {
        return false;
    }

    this->setPadding(PADDING);
    infoBg->setColor(LIGHT_BROWN_3B);
    infoBg->addChild(menu);
    pauseItem->setPosition({ menuSize.width / 2, menuSize.height - buttonSize.height / 2 - PADDING });
    pauseItem->setSizeMult(pauseItem->m_scaleMultiplier = 1.1f);
    sendItem->setPosition({ menuSize.width / 2, buttonSize.height / 2 + PADDING });
    sendItem->setSizeMult(sendItem->m_scaleMultiplier = 1.1f);
    menu->setContentSize(menuSize);
    menu->setPosition(ZERO_POINT);
    menu->addChild(pauseItem);
    menu->addChild(sendItem);

    return true;
}

void ControlMenu::onPause(CCObject* sender) {
    const bool value = !Mod::get()->getSettingValue<bool>("pause-requests");

    m_settings = nullptr;
    m_pauseButton->setString(value ? "Resume" : "Pause");
    Mod::get()->setSettingValue("pause-requests", value);

    if (!value) {
        ProxyHandler::resumeRequests();
    }
}

void ControlMenu::onSend(CCObject* sender) {
    // Mod::get()->sendRequest();
}