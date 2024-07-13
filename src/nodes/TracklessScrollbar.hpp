#pragma once

#include "../include.hpp"
#include "../objects/ThemeStyle.hpp"

class TracklessScrollbar : public Scrollbar {
public:
    static TracklessScrollbar* create(const CCSize& size, ListView* list);
private:
    bool init(const CCSize& size, ListView* list);
    void update(float delta) override;
    void draw() override;
};