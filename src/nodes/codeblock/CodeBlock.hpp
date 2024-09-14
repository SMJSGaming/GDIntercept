#pragma once

#include <fmt/chrono.h>
#include "../../include.hpp"
#include "SideBar.hpp"
#include "cells/ViewCell.hpp"
#include "cells/ActionCell.hpp"
#include "cells/CategoryCell.hpp"
#include "utils/CullingList.hpp"
#include "cells/CullingCell.hpp"
#include "cells/CodeLineCell.hpp"
#include "../utils/KeybindNode.hpp"
#include "utils/TracklessScrollbar.hpp"
#include "../../objects/ThemeStyle.hpp"
#include "../../objects/JSONTokenizer.hpp"

class CodeBlock : public KeybindNode<Border>, public FLAlertLayerProtocol {
public:
    static CodeBlock* create(const CCSize& size);

    void reloadCode();
    void reloadSideBar();
    void updateInfo(HttpInfo* info);
    void setCode(const HttpInfo::HttpContent& code);
    HttpInfo* getActiveInfo() const;
private:
    static const std::vector<SideBarCell::SideBarView> views;
    static const SideBar::Categories actions;

    HttpInfo* m_info;
    SideBar* m_bar;
    std::string m_code;
    std::tuple<TracklessScrollbar*, TracklessScrollbar*> m_scrollbars;
    CCLayerColor* m_corner;

    CodeBlock();
    void setup() override;
    bool init(const CCSize& size);
    CCSize getTrueFontSize();
    float getCellHeight();
    void showMessage(const std::string& message, const ccColor3B& color = { 0xFF, 0xFF, 0xFF });
    void scroll(const float x, const float y);
    bool onCopy();
    bool onSend();
    bool onCancel();
    bool onOpenSaveFiles();
    bool onOpenConfigFiles();
    bool onSave();
    bool onPause(const SideBarCell::OriginalCallback& original);
    bool onResume(const SideBarCell::OriginalCallback& original);
    bool onRaw(const SideBarCell::OriginalCallback& original);
    bool onFormatted(const SideBarCell::OriginalCallback& original);
    void onInfo();
    void onBody();
    void onQuery();
    void onRequestHeaders();
    void onResponse();
    void onResponseHeaders();
    void FLAlert_Clicked(FLAlertLayer* alert, const bool state) override;
};