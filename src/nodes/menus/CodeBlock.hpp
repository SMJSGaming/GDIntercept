#pragma once

#include "../../include.hpp"
#include "../ButtonBar.hpp"
#include "../CopyButton.hpp"
#include "../utils/CullingList.hpp"
#include "../cells/CodeLineCell.hpp"
#include "../../objects/ThemeStyle.hpp"
#include "../../objects/JSONTokenizer.hpp"
#include "../utils/TracklessScrollbar.hpp"

class CodeBlock : public Border {
public:
    static size_t buttonCount;

    static CodeBlock* create(const CCSize& size, const CCSize& buttonBarSize);

    void updateInfo(const HttpInfo* info);
    void setCode(const HttpInfo::HttpContent& code);
private:
    static LookupTable<char, SEL_MenuHandler> dataTypes;
    static char currentDataType;

    const HttpInfo* m_info;
    std::unordered_map<char, CCLabelBMFont*> m_buttons;
    std::string m_code;

    bool init(const CCSize& size, const CCSize& buttonBarSize);
    CCSize getTrueFontSize();
    float getCellHeight();
    void updateDataTypeColor(const char type);
    void scroll(const float delta);
    void onCopy();
    void onBody(CCObject* sender);
    void onQuery(CCObject* sender);
    void onHeaders(CCObject* sender);
    void onResponse(CCObject* sender);
    void draw() override;
};