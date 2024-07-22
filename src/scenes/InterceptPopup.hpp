#pragma once

#include <Geode/ui/GeodeUI.hpp>
#include "../include.hpp"
#include "../nodes/ButtonBar.hpp"
#include "../objects/ThemeStyle.hpp"
#include "../nodes/menus/InfoArea.hpp"
#include "../nodes/menus/CodeBlock.hpp"
#include "../nodes/menus/CaptureList.hpp"
#include "../nodes/menus/ControlMenu.hpp"

class InterceptPopup : public Popup<> {
public:
    static InterceptPopup* get();
    static void scene();

    void reload();
protected:
    bool setup() override;
    void onClose(CCObject* obj) override;
private:
    static CCSize uiSize;
    static float uiPadding;
    static float captureCellWidth;
    static float captureCellHeight;
    static float infoWidth;
    static float infoRowHeight;
    static float middleColumnXPosition;
    static float codeBlockButtonHeight;

    InfoArea* m_infoArea;
    CodeBlock* m_codeBlock;
    ControlMenu* m_controls;
    CaptureList* m_list;
    FLAlertLayer* m_settings;

    InfoArea* setupInfo();
    CaptureList* setupList();
    ControlMenu* setupControls();
    CodeBlock* setupCodeBlock();
    float getPageHeight();
    float getComponentYPosition(float offset, float itemHeight);
    void onSettings(CCObject* obj);
};