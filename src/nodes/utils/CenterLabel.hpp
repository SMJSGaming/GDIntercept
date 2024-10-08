#pragma once

#include "../../include.hpp"

class CenterLabel : public CCLabelBMFont {
public:
    static CenterLabel* create(const std::string& string, const std::string& font);
protected:
    bool init(const std::string& string, const std::string& font);
    void updateLabel() override;
};