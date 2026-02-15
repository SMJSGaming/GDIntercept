#include "MonospaceLabel.hpp"

MonospaceLabel* MonospaceLabel::create(const std::string_view string, const std::string_view font, const float scale) {
    MonospaceLabel* instance = new MonospaceLabel();

    if (instance && instance->init(string, font, scale)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CCSize MonospaceLabel::getCharacterSize(const std::string_view font) {
    return CCLabelBMFont::create("M", font.data())->getContentSize();
}

bool MonospaceLabel::init(const std::string_view string, const std::string_view font, const float scale) {
    ESCAPE_WHEN(!this->initWithString(string.data(), font.data()), false);

    this->setScale(scale);

    return true;
}

float MonospaceLabel::getCharacterWidth() {
    return MonospaceLabel::getCharacterSize(this->getFntFile()).width;
}

void MonospaceLabel::updateLabel() {
    CCLabelBMFont::updateLabel();
    const float characterWidth = this->getCharacterWidth();
    const size_t count = this->getChildrenCount();

    for (size_t i = 0; i < count; i++) {
        this->getChildByIndex(i)->setPositionX((i + 0.5f) * characterWidth);
    }
}