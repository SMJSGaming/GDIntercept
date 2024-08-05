#pragma once

#include "../../include.hpp"

class Character : public CCLabelBMFont {
public:
    static Character* create(const char character, const std::string& font);

    void setCString(const char* label) override;
private:
    bool initWithString(const char character, const std::string& font);
    void update(float delta) override;
};