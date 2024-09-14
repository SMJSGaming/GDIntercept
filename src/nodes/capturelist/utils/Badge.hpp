#pragma once

#include "../../../include.hpp"
#include "Tooltip.hpp"
#include "../../utils/HoverNode.hpp"

class Badge : public HoverNode<> {
public:
    static Badge* create(const std::string& name, CCNode* node, const std::function<void(GLubyte)>& opacityCallback);
private:
    std::string m_name;
    CCNode* m_node;
    Tooltip* m_tooltip;
    std::function<void(GLubyte)> m_opacityCallback;

    Badge(const std::string& name, CCNode* node, const std::function<void(GLubyte)>& opacityCallback);
    bool init() override;
    void onHover() override;
    void unHover() override;
};