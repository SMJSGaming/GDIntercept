#pragma once

#include "../include.hpp"
#include "../objects/ThemeStyle.hpp"

struct TracklessScrollbar : public Scrollbar {
    static TracklessScrollbar* create(const CCSize& size, ListView* list);
private:
    bool init(const CCSize& size, ListView* list);
    void draw() override;
};