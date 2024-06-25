#include "ButtonBar.hpp"

ButtonBar* ButtonBar::create(const char* bgSprite, const float spriteScale, const CCSize& size, const std::vector<CCMenuItemSpriteExtra*>& items) {
    ButtonBar* instance = new ButtonBar(items.size());

    if (instance && instance->init(bgSprite, spriteScale, size, items)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

ButtonBar::ButtonBar(const size_t itemCount) : m_itemCount(itemCount) {}

bool ButtonBar::init(const char* bgSprite, const float spriteScale, const CCSize& size, const std::vector<CCMenuItemSpriteExtra*>& items) {
    if (!CCNode::init()) {
        return false;
    }

    CCScale9Sprite* sprite = CCScale9Sprite::create(bgSprite);
    CCMenu* menu = CCMenu::create();

    this->setContentSize(size);

    for (size_t i = 0; i < m_itemCount; i++) {
        const float width = size.width / m_itemCount;
        CCMenuItemSpriteExtra* item = items.at(i);

        item->setContentSize({ width, size.height });
        item->setPosition({ (i + 0.5f) * width, size.height / 2 });
        item->setAnchorPoint(CENTER);
        menu->addChild(item);
    }

    sprite->setScale(spriteScale);
    sprite->setScaledContentSize(size);
    sprite->setAnchorPoint(BOTTOM_LEFT);
    sprite->setOpacity(70);
    menu->setPosition(ZERO_POINT);
    menu->setAnchorPoint(BOTTOM_LEFT);
    this->addChild(sprite);
    this->addChild(menu);

    return true;
}

void ButtonBar::draw() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const float width = this->getContentWidth() / m_itemCount;
    const float height = this->getContentHeight();

    CCNode::draw();

    ccDrawColor4B(theme.line.r, theme.line.g, theme.line.b, 40);
    glLineWidth(2);

    for (size_t i = 1; i < m_itemCount; i++) {
        const float x = i * width;

        ccDrawLine({ x, 0 }, { x, height });
    }
}
