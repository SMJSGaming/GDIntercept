#pragma once

#include "../../include.hpp"

class RescalingNode : public CCNode {
public:
    static RescalingNode* create(CCNode* node, const CCSize& size);

    RescalingNode();
    bool init(CCNode* node, const CCSize& size);
    void setNode(CCNode* node);
    void setRescaleWidth(const bool value);
    void setRescaleHeight(const bool value);
    void setContentSize(const CCSize& size) override;
private:
    GETTER(CCNode*, node, Node);
    GETTER(bool, rescaleWidth, RescaleWidth);
    GETTER(bool, rescaleHeight, RescaleHeight);
};