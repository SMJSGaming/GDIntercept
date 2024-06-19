#pragma once

#include "../../include.hpp"
#include "../BorderFix.hpp"
#include "../cells/CodeLineCell.hpp"
#include "../TracklessScrollbar.hpp"
#include "../../objects/HttpInfo.hpp"
#include "../../objects/JSONColor.hpp"
#include "../../objects/ThemeStyle.hpp"

struct JSONCodeBlock : public BorderFix {
    static JSONCodeBlock* create(const std::pair<HttpInfo::ContentType, std::string>& code, const CCSize& size, bool readonly = true);

    void copyCode();
    void setCode(const std::pair<HttpInfo::ContentType, std::string>& code);
private:
    std::string m_code;

    bool init(const std::pair<HttpInfo::ContentType, std::string>& code, const CCSize& size, bool readonly);
    void draw() override;
};