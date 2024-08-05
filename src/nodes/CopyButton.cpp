#include "CopyButton.hpp"

CopyButton* CopyButton::create(const CCSize& size, const std::function<void()>& copyCallback) {
    CopyButton* instance = new CopyButton(copyCallback);

    if (instance && instance->init(size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CopyButton::CopyButton(const std::function<void()>& copyCallback) : m_copyCallback(copyCallback) { }

bool CopyButton::init(const CCSize& size) {
    CCSprite* copyIcon = CCSprite::create("GJ_button_01.png");
    Character* copyLabel = Character::create('C', "bigFont.fnt");

    if (!CCMenuItemSpriteExtra::init(copyIcon, nullptr, this, menu_selector(CopyButton::onCopy))) {
        return false;
    }

    this->setContentSize(size);
    copyIcon->setPosition(this->getContentSize() / 2);
    copyIcon->setScale(size.height / copyIcon->getContentHeight());
    copyLabel->setPosition(copyIcon->getContentSize() / 2);
    copyLabel->setScale((size.height * 0.8f) / copyLabel->getContentHeight() / copyIcon->getScale());
    copyIcon->addChild(copyLabel);

    return true;
}

void CopyButton::onCopy(CCObject* sender) {
    m_copyCallback();
}