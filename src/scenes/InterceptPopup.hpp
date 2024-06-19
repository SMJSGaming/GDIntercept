#pragma once

#include "../include.hpp"
#include "../nodes/ButtonBar.hpp"
#include "../nodes/BorderFix.hpp"
#include "../objects/HttpInfo.hpp"
#include "../objects/ThemeStyle.hpp"
#include "../nodes/lists/CaptureList.hpp"
#include "../nodes/lists/JSONCodeBlock.hpp"

struct InterceptPopup : public Popup<> {
    static InterceptPopup* get();
    static void scene();
protected:
    bool setup() override;
    void onClose(CCObject* obj) override;
private:
    static InterceptPopup* instance;
    static float uiWidth;
    static float uiHeight;
    static float uiPadding;
    static float captureCellWidth;
    static float captureCellHeight;
    static float infoWidth;
    static float infoRowHeight;
    static float middleColumnXPosition;
    static float codeBlockButtonWidth;
    static float codeBlockButtonHeight;
    static std::vector<std::tuple<char, SEL_MenuHandler>> dataTypes;
    static std::vector<std::tuple<std::string, std::string, SEL_MenuHandler>> toggles;

    CaptureList* m_captureList;
    HttpInfo* m_currentRequest;
    char m_currentDataType;

    void setupList();
    void setupSettings();
    void setupInfo();
    void setupCodeBlock();
    void updateInfo(HttpInfo* request);
    float getPageHeight();
    float getYPadding();
    float getComponentYPosition(float offset, float itemHeight);
    void updateDataTypeColor(char type);
    void onPauseRequest(CCObject* sender);
    void onCensorData(CCObject* sender);
    void onRememberRequests(CCObject* sender);
    void onRawData(CCObject* sender);
    void onBody(CCObject* sender);
    void onQuery(CCObject* sender);
    void onHeaders(CCObject* sender);
    void onResponse(CCObject* sender);
};