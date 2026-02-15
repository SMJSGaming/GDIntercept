#include "Warning.hpp"

CCScale9Sprite* Warning::WARNING = nullptr;

void Warning::show() {
    ESCAPE_WHEN(Warning::WARNING,);
    const CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    Warning::WARNING = CCScale9Sprite::create("square02_001.png");
    CCLabelBMFont* label = CCLabelBMFont::create("Requests Paused", "bigFont.fnt");

    label->setScale(2);
    Warning::WARNING->setScale(0.25f);
    Warning::WARNING->setOpacity(0x7F);
    Warning::WARNING->setAnchorPoint(TOP_CENTER);
    Warning::WARNING->setContentSize(label->getScaledContentSize() + ccp(PADDING, PADDING) * 4);
    Warning::WARNING->setPosition({ winSize.width / 2, winSize.height - PADDING });
    label->setPosition(Warning::WARNING->getContentSize() / 2);
    Warning::WARNING->addChild(label);

    ProxyHandler::pauseAll();
    OverlayManager::get()->addChild(Warning::WARNING);
}

void Warning::hide() {
    ESCAPE_WHEN(!Warning::WARNING,);
    Warning::WARNING->removeFromParent();
    Warning::WARNING = nullptr;
}