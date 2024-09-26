#include "Warning.hpp"

CCScale9Sprite* Warning::warning = nullptr;

void Warning::show() {
    ESCAPE_WHEN(Warning::warning,);
    const CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    Warning::warning = CCScale9Sprite::create("square02_001.png");
    CCLabelBMFont* label = CCLabelBMFont::create("Requests Paused", "bigFont.fnt");

    label->setScale(2);
    Warning::warning->setScale(0.25f);
    Warning::warning->setOpacity(0x7F);
    Warning::warning->setAnchorPoint(TOP_CENTER);
    Warning::warning->setContentSize(label->getScaledContentSize() + ccp(PADDING, PADDING) * 4);
    Warning::warning->setPosition({ winSize.width / 2, winSize.height - PADDING });
    label->setPosition(Warning::warning->getContentSize() / 2);
    Warning::warning->addChild(label);

    ProxyHandler::pauseAll();
    SceneManager::get()->keepAcrossScenes(Warning::warning);
}

void Warning::hide() {
    ESCAPE_WHEN(!Warning::warning,);
    SceneManager::get()->forget(Warning::warning);
    Warning::warning->removeFromParent();
    Warning::warning = nullptr;
}