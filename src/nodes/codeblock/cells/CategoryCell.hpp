#pragma once

#include "../../../include.hpp"
#include "../../utils/HoverNode.hpp"
#include "../../utils/CenterLabel.hpp"
#include "../utils/SharedWidthNode.hpp"
#include "../../utils/RescalingNode.hpp"

class CategoryCell;

namespace SideBarCell {
    struct SideBarCategory {
        bool operator ==(const SideBarCategory& other) const;

        const std::string name;
        const std::string icon;

        CategoryCell* toCategoryCell() const;
    };
}

class CategoryCell : public SharedWidthNode<HoverNode<CCLayerColor>> {
public:
    static CategoryCell* create(const SideBarCell::SideBarCategory& view);
private:
    RescalingNode* m_icon;
    CCLabelBMFont* m_name;

    bool init(const SideBarCell::SideBarCategory& view);
    void onScaleToMin() override;
    void onScaleToMax() override;
    void onHover() override;
    void draw() override;
};