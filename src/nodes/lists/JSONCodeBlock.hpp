#pragma once

#include "../../include.hpp"
#include "../cells/CodeLineCell.hpp"
#include "../TracklessScrollbar.hpp"
#include "../../objects/JSONColor.hpp"
#include "../../objects/ThemeStyle.hpp"

struct JSONCodeBlock : public Border {
    void copyCode();
    void setCode(const HttpInfo::HttpContent& code);
protected:
    bool init(const HttpInfo::HttpContent& code, const CCSize& size);
private:
    std::string m_code;

    void draw() override;
};