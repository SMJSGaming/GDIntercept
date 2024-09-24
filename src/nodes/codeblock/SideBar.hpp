#pragma once

#include "../../include.hpp"
#include "cells/ViewCell.hpp"
#include "cells/ActionCell.hpp"
#include "cells/CategoryCell.hpp"
#include "../utils/KeybindNode.hpp"
#include "utils/SharedWidthNode.hpp"

class CodeBlock;

class SideBar : public KeybindNode<SharedWidthNode<CCLayerColor>> {
public:
    using Categories = LookupTable<SideBarCell::SideBarCategory, std::vector<SideBarCell::SideBarAction>>;

    static SideBar* create(
        CodeBlock* block,
        const float height,
        const std::vector<SideBarCell::SideBarView>& views,
        const Categories& actions
    );

    void reloadView();
    void reloadState();
    float overrideWidths() = delete;
private:
    struct ActionID {
        const bool isOpen;
        std::string category;
        size_t index;
        size_t cellIndex;

        ActionID(const std::string& id);
        SideBarCell::SideBarCategory getCategory(const Categories& actions) const;
    };

    static const SideBarCell::SideBarAction openAction;
    static size_t activeView;
    const std::vector<SideBarCell::SideBarView> m_views;
    const Categories m_actions;
    std::vector<SharedWidthNode<HoverNode<CCLayerColor>>*> m_cells;
    std::vector<ViewCell*> m_viewCells;
    std::vector<ActionCell*> m_actionCells;
    ActionCell* m_openCell;
    CodeBlock* m_block;
    CCMenu* m_menu;

    SideBar(CodeBlock* block, const std::vector<SideBarCell::SideBarView>& views, const Categories& actions);
    bool init(const float height);
    void setup() override;
    CCMenuItemSpriteExtra* createMenuItem(SharedWidthNode<HoverNode<CCLayerColor>>* cell, const SEL_MenuHandler selector);
    void switchView(CCObject* sender);
    void executeAction(CCObject* sender);
    void onScaleToMin() override;
    void onScaleToMax() override;
    void draw() override;
};