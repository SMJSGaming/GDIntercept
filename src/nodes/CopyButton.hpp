#pragma once

#include "../include.hpp"
#include "utils/Character.hpp"

class CopyButton : public CCMenuItemSpriteExtra {
public:
    static CopyButton* create(const CCSize& size, const std::function<void()>& copyCallback);
private:
    std::function<void()> m_copyCallback;

    CopyButton(const std::function<void()>& copyCallback);
    bool init(const CCSize& size);
    void onCopy(CCObject* sender);
};