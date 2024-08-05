#pragma once

#include "../../include.hpp"
#include "../../objects/ThemeStyle.hpp"

class TracklessScrollbar : public Scrollbar {
public:
    static TracklessScrollbar* create(const CCSize& size, CCScrollLayerExt* scrollLayer);
private:
    bool init(const CCSize& size, CCScrollLayerExt* scrollLayer);
    void update(float delta) override;
    void draw() override;
};