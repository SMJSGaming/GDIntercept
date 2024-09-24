#pragma once

#include "../../../include.hpp"

class TracklessScrollbar : public Scrollbar {
public:
    static TracklessScrollbar* create(const bool vertical, const CCSize& size, CCScrollLayerExt* scrollLayer);
private:
    const bool m_vertical;

    TracklessScrollbar(const bool vertical);
    bool init(const CCSize& size, CCScrollLayerExt* scrollLayer);
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void update(const float dt) override;
    void draw() override;
};