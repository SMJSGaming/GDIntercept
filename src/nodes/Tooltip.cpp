#include "Tooltip.hpp"

Tooltip* Tooltip::create(const std::string& text, const float scale, const GLubyte opacity) {
    CCLabelBMFont* label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    Tooltip* instance = new Tooltip();

    label->setScale(scale);

    CCSize labelSize = label->getScaledContentSize() + ccp(PADDING, PADDING) * 2;

    if (instance && instance->initAnchored(labelSize.width, labelSize.height, label, opacity, "square02_001.png")) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool Tooltip::setup(CCLabelBMFont* label, const GLubyte opacity) {
    this->ignoreAnchorPointForPosition(false);
    this->setContentSize(m_size);
    this->setOpacity(0);
    m_mainLayer->setPosition(m_size * m_mainLayer->getAnchorPoint());
    m_bgSprite->setScale(0.2f);
    m_bgSprite->setOpacity(opacity);
    m_bgSprite->setScaledContentSize(m_size);
    m_bgSprite->setPosition(m_size * m_bgSprite->getAnchorPoint());
    label->setPosition(this->getContentSize() / 2);
    m_mainLayer->addChild(label);
    m_buttonMenu->removeFromParent();

    return true;
}