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
    PRIMITIVE_GETTER(CCNode*, node, Node);
    BOOL_GETTER(rescaleWidth, RescaleWidth);
    BOOL_GETTER(rescaleHeight, RescaleHeight);
};