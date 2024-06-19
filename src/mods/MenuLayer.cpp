#include "MenuLayer.hpp"

bool ModMenuLayer::init() {
    if (!MenuLayer::init()) {
        return false;
    }

    CCSprite* logo = CCSprite::create("GDI-round.png"_spr);

    logo->setScale(0.4f);

    CCMenu* menu = CCMenu::createWithItem(CCMenuItemSpriteExtra::create(logo, this, menu_selector(ModMenuLayer::onLogoClick)));

    menu->setPosition({ 18, CCDirector::sharedDirector()->getWinSize().height - 55 });
    this->addChild(menu);

    return true;
}

void ModMenuLayer::onLogoClick(CCObject* sender) {
    InterceptPopup::scene();
}