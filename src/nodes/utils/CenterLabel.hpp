#pragma once

#include "../../include.hpp"

class CenterLabel : public CCLabelBMFont {
public:
    static CenterLabel* create(const std::string_view string, const std::string_view font);
protected:
    bool init(const std::string_view string, const std::string_view font);
    void updateLabel() override;
};