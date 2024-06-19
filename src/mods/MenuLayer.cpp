#include "MenuLayer.hpp"

bool ModMenuLayer::init() {
    if (!MenuLayer::init()) {
        return false;
    }

    CCNode* logo = createModLogo(Mod::get());

    logo->setScale(0.5f);

    CCMenu* menu = CCMenu::createWithItem(CCMenuItemSpriteExtra::create(logo, this, menu_selector(ModMenuLayer::onLogoClick)));

    menu->setPosition({ 18, CCDirector::sharedDirector()->getWinSize().height - 55 });
    this->addChild(menu);

    return true;
}

void ModMenuLayer::onLogoClick(CCObject* sender) {
    InterceptPopup::scene();
}