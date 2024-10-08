#pragma once

#include <Geode/ui/GeodeUI.hpp>
#include "../include.hpp"
#include "codeblock/CodeBlock.hpp"
#include "capturelist/CaptureList.hpp"

class InterceptPopup : public Popup<> {
public:
    static InterceptPopup* get();
    static void scene();

    void reloadList();
    void reloadCodeBlock(const bool recycleInfo);
    void reloadCode();
    void reloadSideBar();
protected:
    bool setup() override;
private:
    static CCSize popupPadding;
    static float uiPadding;
    static float captureCellHeight;
    static float captureCellBadgeHeight;

    float m_captureCellWidth;
    float m_leftColumnXPosition;
    bool m_settingsShouldReload;
    CodeBlock* m_codeBlock;
    CaptureList* m_list;
    FLAlertLayer* m_settings;

    InterceptPopup(const CCSize& size);
    void preReload();
    void postReload();
    void setupList();
    void setupCodeBlock(const bool recycleInfo = false);
    float getPageHeight();
    float getComponentYPosition(const float offset, const float itemHeight);
    void onSettings(CCObject* obj);
};