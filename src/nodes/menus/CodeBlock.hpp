#pragma once

#include "../../include.hpp"
#include "../ButtonBar.hpp"
#include "../CullingList.hpp"
#include "../TracklessScrollbar.hpp"
#include "../cells/CodeLineCell.hpp"
#include "../../objects/ThemeStyle.hpp"
#include "../../objects/JSONTokenizer.hpp"

class CodeBlock : public Border {
public:
    static size_t buttonCount;

    static CodeBlock* create(const CCSize& size, const CCSize& buttonBarSize);

    void updateInfo(HttpInfo* info);
    void setCode(const HttpInfo::HttpContent& code);
private:
    static std::vector<std::pair<char, SEL_MenuHandler>> dataTypes;
    static char currentDataType;

    std::unordered_map<char, CCLabelBMFont*> m_buttons;
    HttpInfo* m_info;
    std::string m_code;

    bool init(const CCSize& size, const CCSize& buttonBarSize);
    CCSize getTrueFontSize();
    float getCellHeight();
    void updateDataTypeColor(const char type);
    void scroll(const float delta);
    void onBody(CCObject* sender);
    void onQuery(CCObject* sender);
    void onHeaders(CCObject* sender);
    void onResponse(CCObject* sender);
    void draw() override;
};