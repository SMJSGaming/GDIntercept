#pragma once

#include "../../include.hpp"

struct TouchFixList : public ListView {
    static TouchFixList* create(CCArray* cells, const float cellHeight, const float width, const float height);
private:
    void draw() override;
};