#pragma once

#include <Geode/ui/GeodeUI.hpp>
#include "../include.hpp"
#include "../nodes/InfoArea.hpp"
#include "../nodes/ButtonBar.hpp"
#include "../nodes/CodeBlock.hpp"
#include "../objects/HttpInfo.hpp"
#include "../objects/ThemeStyle.hpp"
#include "../nodes/lists/CaptureList.hpp"
#include "../nodes/lists/JSONCodeBlock.hpp"

struct InterceptPopup : public Popup<> {
    static InterceptPopup* get();
    static void scene();

    void reload();
    void copyCode();
protected:
    bool setup() override;
    void onClose(CCObject* obj) override;
private:
    static float uiWidth;
    static float uiHeight;
    static float uiPadding;
    static float captureCellWidth;
    static float captureCellHeight;
    static float infoWidth;
    static float infoRowHeight;
    static float middleColumnXPosition;
    static float codeBlockButtonHeight;

    InfoArea* m_infoArea;
    CodeBlock* m_codeBlock;
    Border* m_settings;
    Border* m_list;

    InfoArea* setupInfo();
    Border* setupList();
    Border* setupSettings();
    CodeBlock* setupCodeBlock();
    float getPageHeight();
    float getComponentYPosition(float offset, float itemHeight);
    void onSettings(CCObject* obj);
};