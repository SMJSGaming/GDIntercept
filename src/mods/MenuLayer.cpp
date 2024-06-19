#include "MenuLayer.hpp"

bool ModMenuLayer::init() {
    if (!MenuLayer::init()) {
        return false;
    }

    CCSprite* logo = CCSprite::create("GDI-round.png"_spr);

    logo->setScale(0.4f);

    CCMenuItemSpriteExtra* logoBtn = CCMenuItemSpriteExtra::create(logo, this, menu_selector(ModMenuLayer::onLogoClick));
    CCNode* closeMenu = this->getChildByID("close-menu");

    closeMenu->addChild(logoBtn);
    closeMenu->updateLayout();

    return true;
}

void ModMenuLayer::onLogoClick(CCObject* sender) {
    InterceptPopup::scene();
}