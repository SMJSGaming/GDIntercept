#include "CenterLabel.hpp"

CenterLabel* CenterLabel::create(const std::string& string, const std::string& font) {
    CenterLabel* instance = new CenterLabel();

    if (instance && instance->initWithString(string, font)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CenterLabel::initWithString(const std::string& string, const std::string& font) {
    ESCAPE_WHEN(!CCLabelBMFont::initWithString(string.c_str(), font.c_str()), false);

    this->scheduleUpdate();

    return true;
}

void CenterLabel::update(const float dt) {
    CCLabelBMFont* referenceLabel = CCLabelBMFont::create("A", this->getFntFile());

    referenceLabel->setScale(this->getScale());

    CCSprite* referenceSprite = cocos::getChild<CCSprite>(this, 0);
    const size_t count = this->getChildrenCount();
    const float heightAddition = referenceLabel->getContentHeight() / 2 - referenceSprite->getPositionY();

    for (int i = 0; i < count; i++) {
        CCNode* child = cocos::getChild(this, i);

        child->setPositionY(child->getPositionY() + heightAddition);
    }
}