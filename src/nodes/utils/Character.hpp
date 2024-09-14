#pragma once

#include "../../include.hpp"
#include "CenterLabel.hpp"

class Character : public CenterLabel {
public:
    static Character* create(const char character, const std::string& font, const bool rescale = false);

    void setCString(const char* label) override;
protected:
    void update(const float dt) override;
private:
    bool m_rescale;

    Character(const bool rescale);
};