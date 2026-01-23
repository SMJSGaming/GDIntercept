#pragma once

#include "../../../include.hpp"

class MonospaceLabel : public CCLabelBMFont {
public:
    static MonospaceLabel* create(const std::string_view string, const std::string_view font, const float scale = 1);
    static CCSize getCharacterSize(const std::string_view font);

    float getCharacterWidth();
private:
    bool init(const std::string_view string, const std::string_view font, const float scale);
    void updateLabel() override;
};