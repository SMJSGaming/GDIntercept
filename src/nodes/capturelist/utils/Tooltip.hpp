#pragma once

#include "../../../include.hpp"

class Tooltip : public Popup {
public:
    static Tooltip* create(const std::string_view text, const float scale, const GLubyte opacity = 255);
protected:
    bool init(const std::string_view text, const float scale, const GLubyte opacity);
};