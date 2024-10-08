#include "Character.hpp"

Character* Character::create(const char character, const std::string& font, const bool rescale) {
    Character* instance = new Character(rescale);

    if (instance && instance->init(std::string(1, character), font)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

Character::Character(const bool rescale) : m_rescale(rescale) {}

void Character::setCString(const char* label) {
    CenterLabel::setCString(std::string(label).substr(0, 1).c_str());
}

void Character::updateLabel() {
    if (m_rescale) {
        CCLabelBMFont::updateLabel();
        CCNode* characterNode = cocos::getChild(this, 0);

        characterNode->setScale(this->getContentHeight() / characterNode->getContentHeight());
        characterNode->setPositionY(this->getContentHeight() / 2);
    } else {
        CenterLabel::updateLabel();
    }
}