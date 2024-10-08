#pragma once

#include "../../../include.hpp"

class MonospaceLabel : public CCLabelBMFont {
public:
    static MonospaceLabel* create(const std::string& string, const std::string& font, const float scale = 1);
    static CCSize getCharacterSize(const std::string& font);

    float getCharacterWidth();
private:
    bool init(const std::string& string, const std::string& font, const float scale);
    void updateLabel() override;
};