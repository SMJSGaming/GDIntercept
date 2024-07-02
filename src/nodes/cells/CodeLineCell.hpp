#pragma once

#include "../../include.hpp"
#include "../../objects/JSONColor.hpp"
#include "../../objects/ThemeStyle.hpp"

struct CodeLineCell : public CCLayer {
    static CodeLineCell* create(const HttpInfo::HttpContent& code, const size_t lineNumber, const float lineNumberWidth, JSONColor& color);

    JSONColor& m_color;
private:
    float m_lineNumberWidth;

    CodeLineCell(const float lineNumberWidth, JSONColor& color);
    bool init(const HttpInfo::ContentType content, std::string code, const size_t lineNumber);
};