#include "CenterLabel.hpp"

CenterLabel* CenterLabel::create(const std::string_view string, const std::string_view font) {
    CenterLabel* instance = new CenterLabel();

    if (instance && instance->initWithString(StringBuffer(string).c_str(), StringBuffer(font).c_str())) {
        instance->autorelease();

        return instance;
    } else {
        delete instance;
        return nullptr;
    }
}

void CenterLabel::updateLabel() {
    CCLabelBMFont::updateLabel();
    CCLabelBMFont* referenceLabel = CCLabelBMFont::create("0", this->getFntFile());
    const size_t count = this->getChildrenCount();

    referenceLabel->setScale(this->getScale());

    if (count) {
        CCSprite* referenceSprite = this->getChildByIndex<CCSprite>(0);
        const float heightAddition = referenceLabel->getContentHeight() / 2 - referenceSprite->getPositionY();

        for (int i = 0; i < count; i++) {
            CCNode* child = this->getChildByIndex(i);

            child->setPositionY(child->getPositionY() + heightAddition);
        }
    }
}