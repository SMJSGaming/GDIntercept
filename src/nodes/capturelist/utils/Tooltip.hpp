#pragma once

#include "../../../include.hpp"

class Tooltip : public Popup<CCLabelBMFont*, const GLubyte> {
public:
    static Tooltip* create(const std::string& text, const float scale, const GLubyte opacity = 255);
protected:
    bool setup(CCLabelBMFont* label, const GLubyte opacity) override;
};