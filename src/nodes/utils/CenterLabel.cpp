#include "CenterLabel.hpp"

CenterLabel* CenterLabel::create(const std::string_view string, const std::string_view font) {
    CenterLabel* instance = new CenterLabel();

    if (instance && instance->init(string, font)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CenterLabel::init(const std::string_view string, const std::string_view font) {
    ESCAPE_WHEN(!this->initWithString(StringBuffer(string).c_str(), StringBuffer(font).c_str()), false);

    return true;
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