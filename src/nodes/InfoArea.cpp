#include "InfoArea.hpp"

InfoArea* InfoArea::create(const CCSize& size) {
    InfoArea* instance = new InfoArea();

    if (instance && instance->init(size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool InfoArea::init(const CCSize& size) {
    SimpleTextArea* infoText = SimpleTextArea::create("", "chatFont.fnt", 0.5f, size.width - PADDING * 4);
    CCScale9Sprite* infoBg = CCScale9Sprite::create("square02b_001.png");

    if (!BorderFix::init(infoBg, LIGHTER_BROWN_4B, size)) {
        return false;
    }

    this->setPadding(PADDING);
    infoBg->addChild(infoText);
    infoBg->setColor(LIGHT_BROWN_3B);
    infoText->setID("info_text"_spr);
    infoText->setAnchorPoint(CENTER_LEFT);
    infoText->setPosition({ PADDING, infoBg->getContentHeight() / 2 });
    infoText->setMaxLines(5);
    infoText->setLinePadding(2);
    infoText->setWrappingMode(CUTOFF_WRAP);

    return true;
}

void InfoArea::updateRequest(HttpInfo* request) {
    as<SimpleTextArea*>(this->getNode()->getChildByID("info_text"_spr))->setText(request->generateBasicInfo());
}