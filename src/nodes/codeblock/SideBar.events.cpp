#include "SideBar.hpp"

using namespace SideBarCell;

void SideBar::setup() {
    this->bind("collapse_side_bar"_spr, [this]() {
        Mod* mod = Mod::get();

        mod->setSettingValue("minimize-side-menu", !mod->getSettingValue<bool>("minimize-side-menu"));
    });
}

void SideBar::switchView(CCObject* sender) {
    if (sender != nullptr) {
        SideBar::activeView = sender->getTag();
    } else if (SideBar::activeView >= m_views.size()) {
        SideBar::activeView = 0;
    }

    for (size_t i = 0; i < m_viewCells.size(); i++) {
        m_viewCells.at(i)->setOpacity(i == SideBar::activeView ? 255 : 0);
    }

    m_views.at(SideBar::activeView).callback(m_block);
}

void SideBar::executeAction(CCObject* sender) {
    const ActionID id = as<CCMenuItemSpriteExtra*>(sender)->getID();
    const SideBarAction& action = id.isOpen ? SideBar::openAction : m_actions.at(id.getCategory(m_actions)).at(id.index);
    ActionCell* cell = id.isOpen ? m_openCell : m_actionCells.at(id.cellIndex);

    ESCAPE_WHEN(!cell->isEnabled(),);

    const bool oldState = cell->getState();

    cell->setState(!oldState);
    cell->setOpacity(255);
    m_pScheduler->scheduleSelector(schedule_selector(ActionCell::hideOpacity), cell, 0, 0, 0.2f, false);

    if (oldState) {
        cell->setState(action.on.callback(m_block) ^ oldState);
    } else {
        cell->setState(action.off.callback(m_block) ^ oldState);
    }
}

void SideBar::onScaleToMin() {
    for (SharedWidthNode<HoverNode<CCLayerColor>>* cell : m_cells) {
        cell->scaleToMin();
        cell->getParent()->setContentWidth(m_minWidth);
    }

    m_openCell->setState(true);

    this->setContentWidth(m_minWidth);
    m_menu->setContentWidth(m_minWidth);
}

void SideBar::onScaleToMax() {
    for (SharedWidthNode<HoverNode<CCLayerColor>>* cell : m_cells) {
        cell->scaleToMax();
        cell->getParent()->setContentWidth(m_maxWidth);
    }

    m_openCell->setState(false);

    this->setContentWidth(m_maxWidth);
    m_menu->setContentWidth(m_maxWidth);
}