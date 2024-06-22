#pragma once

#include "../../include.hpp"
#include "TouchFixList.hpp"
#include "../BorderFix.hpp"
#include "../cells/CodeLineCell.hpp"
#include "../TracklessScrollbar.hpp"
#include "../../objects/HttpInfo.hpp"
#include "../../objects/JSONColor.hpp"
#include "../../objects/ThemeStyle.hpp"

struct JSONCodeBlock : public BorderFix {
    void copyCode();
    void setCode(const std::pair<HttpInfo::ContentType, std::string>& code);
protected:
    bool init(const std::pair<HttpInfo::ContentType, std::string>& code, const CCSize& size);
private:
    std::string m_code;

    void draw() override;
};