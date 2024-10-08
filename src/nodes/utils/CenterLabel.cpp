#include "CenterLabel.hpp"

CenterLabel* CenterLabel::create(const std::string& string, const std::string& font) {
    CenterLabel* instance = new CenterLabel();

    if (instance && instance->init(string, font)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CenterLabel::init(const std::string& string, const std::string& font) {
    ESCAPE_WHEN(!this->initWithString(string.c_str(), font.c_str()), false);

    return true;
}

void CenterLabel::updateLabel() {
    CCLabelBMFont::updateLabel();
    CCLabelBMFont* referenceLabel = CCLabelBMFont::create("0", this->getFntFile());
    const size_t count = this->getChildrenCount();

    referenceLabel->setScale(this->getScale());

    if (count) {
        CCSprite* referenceSprite = cocos::getChild<CCSprite>(this, 0);
        const float heightAddition = referenceLabel->getContentHeight() / 2 - referenceSprite->getPositionY();

        for (int i = 0; i < count; i++) {
            CCNode* child = cocos::getChild(this, i);

            child->setPositionY(child->getPositionY() + heightAddition);
        }
    }
}