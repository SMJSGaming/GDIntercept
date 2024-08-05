#include "Character.hpp"

Character* Character::create(const char character, const std::string& font) {
    Character* instance = new Character();

    if (instance && instance->initWithString(character, font)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

void Character::setCString(const char* label) {
    std::string labelStr = label;

    CCLabelBMFont::setCString(labelStr.substr(0, 1).c_str());
}

bool Character::initWithString(const char character, const std::string& font) {
    if (!CCLabelBMFont::initWithString(std::string(1, character).c_str(), font.c_str())) {
        return false;
    }

    this->scheduleUpdate();

    return true;
}

void Character::update(float delta) {
    CCLabelBMFont::update(delta);

    CCNode* characterNode = cocos::getChild(this, 0);

    characterNode->setScale(this->getContentHeight() / characterNode->getContentHeight());
    characterNode->setPositionY(this->getContentHeight() / 2);
}