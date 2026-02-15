#include "Character.hpp"

Character* Character::create(const char character, const std::string_view font, const bool rescale) {
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
    const char buffer[2] = { label[0], 0 };

    CenterLabel::setCString(buffer);
}

void Character::updateLabel() {
    if (m_rescale) {
        CCLabelBMFont::updateLabel();
        CCNode* characterNode = this->getChildByIndex(0);

        characterNode->setScale(this->getContentHeight() / characterNode->getContentHeight());
        characterNode->setPositionY(this->getContentHeight() / 2);
    } else {
        CenterLabel::updateLabel();
    }
}