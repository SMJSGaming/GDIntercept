#pragma once

#include "../../include.hpp"

class ControlMenu : public Border {
public:
    // FYI: I don't like using *& here, geode please expose the settings node
    static ControlMenu* create(const CCSize& size, FLAlertLayer*& settings);

    void updateInfo(const HttpInfo* info);
private:
    const HttpInfo* m_info;
    ButtonSprite* m_pauseButton;
    ButtonSprite* m_sendButton;
    FLAlertLayer*& m_settings;

    ControlMenu(FLAlertLayer*& settings);
    bool init(const CCSize& size);
    void onPause(CCObject* sender);
    void onSend(CCObject* sender);
};