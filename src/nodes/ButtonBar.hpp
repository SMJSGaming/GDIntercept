#pragma once

#include "../include.hpp"
#include "../objects/ThemeStyle.hpp"

class ButtonBar : public CCNode {
public:
    static ButtonBar* create(const char* bgSprite, const float spriteScale, const CCSize& size, const std::vector<CCMenuItemSpriteExtra*>& items);
private:
    size_t m_itemCount;

    ButtonBar(const size_t itemCount);
    bool init(const char* bgSprite, const float spriteScale, const CCSize& size, const std::vector<CCMenuItemSpriteExtra*>& items);
    void draw() override;
};