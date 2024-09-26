#pragma once

#include "../../include.hpp"
#include "SideBar.hpp"
#include "../Warning.hpp"
#include "cells/ViewCell.hpp"
#include "cells/ActionCell.hpp"
#include "cells/CategoryCell.hpp"
#include "utils/CullingList.hpp"
#include "cells/CullingCell.hpp"
#include "cells/CodeLineCell.hpp"
#include "../utils/KeybindNode.hpp"
#include "utils/TracklessScrollbar.hpp"
#include "../../proxy/ProxyHandler.hpp"
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
    static bool acceptedPauses;

    HttpInfo* m_info;
    SideBar* m_bar;
    std::string m_code;
    std::tuple<TracklessScrollbar*, TracklessScrollbar*> m_scrollbars;
    CCLayerColor* m_corner;

    CodeBlock();
    void setup() override;
    bool init(const CCSize& size);
    void showMessage(const std::string& message, const ccColor3B& color = { 0xFF, 0xFF, 0xFF });
    void scroll(const float x, const float y);
    bool onCopy();
    bool onSend();
    bool onCancel();
    bool onOpenSaveFiles();
    bool onOpenThemeFiles();
    bool onSave();
    bool onPause();
    bool onResume();
    bool onRaw(const SideBarCell::OriginalCallback& original);
    bool onFormatted(const SideBarCell::OriginalCallback& original);
    bool onDocs();
    void onInfo();
    void onBody();
    void onQuery();
    void onRequestHeaders();
    void onResponse();
    void onResponseHeaders();
    void FLAlert_Clicked(FLAlertLayer* alert, const bool state) override;
};