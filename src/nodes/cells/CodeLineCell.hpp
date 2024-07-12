#pragma once

#include "../../include.hpp"
#include "../../objects/JSONColor.hpp"
#include "../../objects/ThemeStyle.hpp"

class CodeLineCell : public CCLayer {
public:
    static CodeLineCell* create(const HttpInfo::HttpContent& code, const size_t lineNumber, const float lineNumberWidth, JSONColor& color);
private:
    bool init(const HttpInfo::HttpContent& code, const size_t lineNumber, const float lineNumberWidth, JSONColor& color);
};