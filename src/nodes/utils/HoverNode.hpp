#pragma once

#include "../../include.hpp"

class HoverNode : public CCNode {
protected:
    HoverNode();
    virtual bool init() override;
    float getTrueScale();
    virtual void onHover() = 0;
    virtual void unHover() { };
private:
    float getRecursiveScale(CCNode* node, const float accumulator);
    void update(float delta) override;
    bool m_hovered;
};