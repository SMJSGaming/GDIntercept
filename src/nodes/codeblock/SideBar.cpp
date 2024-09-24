#include "SideBar.hpp"

using namespace SideBarCell;

SideBar::ActionID::ActionID(const std::string& id) : isOpen(id == "open") {
    ESCAPE_WHEN(isOpen,);
    std::stringstream stream(id);
    std::string part;

    for (size_t key = 0; std::getline(stream, part, '-'); key++) {
        if (key == 0) {
            category = part;
        } else if (key == 1) {
            index = std::stoul(part);
        } else {
            cellIndex = std::stoul(part);
        }
    }
}

SideBarCategory SideBar::ActionID::getCategory(const Categories& actions) const {
    for (const auto& [category, _] : actions) {
        if (category.name == this->category) {
            return category;
        }
    }

    return { "", "" };
}

const SideBarAction SideBar::openAction = { "minimize-side-menu", { "", "Close" }, { "open.png"_spr, "close.png"_spr }, true };

SideBar* SideBar::create(CodeBlock* block, const float height, const std::vector<SideBarView>& views, const Categories& actions) {
    SideBar* instance = new SideBar(block, views, actions);

    if (instance && instance->init(height)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

size_t SideBar::activeView = 0;

SideBar::SideBar(CodeBlock* block, const std::vector<SideBarView>& views, const Categories& actions) : m_views(views),
m_actions(actions),
m_block(block) { }

bool SideBar::init(const float height) {
    ESCAPE_WHEN(!KeybindNode::init(), false);
    ESCAPE_WHEN(!CCLayerColor::initWithColor(Theme::getTheme().menu.background), false);

    m_menu = CCMenu::create();
    m_openCell = SideBar::openAction.toActionCell(m_block);
    CCMenuItemSpriteExtra* openItem = this->createMenuItem(m_openCell, menu_selector(SideBar::executeAction));
    size_t actionCount = 0;
    size_t cellIndex = 0;

    this->setContentHeight(height);
    m_menu->setContentHeight(height);
    openItem->setID("open");
    openItem->setAnchorPoint(TOP_LEFT);
    openItem->setPosition({ 0, height });

    for (size_t i = 0; i < m_views.size(); i++) {
        ViewCell* cell = m_views.at(i).toViewCell();
        CCMenuItemSpriteExtra* item = this->createMenuItem(cell, menu_selector(SideBar::switchView));

        item->setTag(i);
        item->setAnchorPoint(TOP_LEFT);
        item->setPosition({ 0, height - item->getContentHeight() * (i + 1) });
        m_viewCells.push_back(cell);
    }

    for (const auto& [_, actions] : m_actions) {
        actionCount += actions.size() + 1;
    }

    for (size_t i = 0; i < m_actions.size(); i++) {
        const auto& [category, actions] = m_actions.at(i);

        CategoryCell* cell = category.toCategoryCell();
        m_minWidth = std::max(m_minWidth, cell->getMinWidth());
        m_maxWidth = std::max(m_maxWidth, cell->getMaxWidth());

        cell->setAnchorPoint(BOTTOM_LEFT);
        cell->setPosition({ 0, cell->getContentHeight() * (actionCount - cellIndex - i - 1) });
        m_cells.push_back(cell);
        m_menu->addChild(cell);

        for (size_t j = 0; j < actions.size(); j++, cellIndex++) {
            ActionCell* cell = actions.at(j).toActionCell(m_block);
            CCMenuItemSpriteExtra* item = this->createMenuItem(cell, menu_selector(SideBar::executeAction));

            item->setID(fmt::format("{}-{}-{}", category.name, j, cellIndex));
            item->setPosition({ 0, item->getContentHeight() * (actionCount - cellIndex - i - 2) });
            item->setAnchorPoint(BOTTOM_LEFT);
            m_actionCells.push_back(cell);
        }
    }

    this->addChild(m_menu);
    m_menu->setPosition(ZERO_POINT);

    for (SharedWidthNode<HoverNode<CCLayerColor>>* cell : m_cells) {
        cell->overrideWidths(m_minWidth, m_maxWidth);
    }

    return true;
}

CCMenuItemSpriteExtra* SideBar::createMenuItem(SharedWidthNode<HoverNode<CCLayerColor>>* cell, const SEL_MenuHandler selector) {
    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(cell, this, selector);

    item->m_animationEnabled = false;

    m_minWidth = std::max(m_minWidth, cell->getMinWidth());
    m_maxWidth = std::max(m_maxWidth, cell->getMaxWidth());

    cell->setPosition(ZERO_POINT);
    cell->setAnchorPoint(BOTTOM_LEFT);
    m_cells.push_back(cell);
    m_menu->addChild(item);

    return item;
}

void SideBar::reloadView() {
    this->switchView(nullptr);
    this->reloadState();
}

void SideBar::reloadState() {
    for (ActionCell* cell : m_actionCells) {
        const ActionID id = cell->getParent()->getID();
        const SideBarAction& action = id.isOpen ? SideBar::openAction : m_actions.at(id.getCategory(m_actions)).at(id.index);

        if (!action.enableState || action.enableState(m_block)) {
            cell->enable();
        } else {
            cell->disable();
        }

        if (action.determineState) {
            cell->setState(action.determineState(m_block));
        }
    }
}

void SideBar::draw() {
    const CCSize size = this->getContentSize();

    CCLayerColor::draw();

    ccDrawColor4B(Theme::getTheme().menu.border);
    ccDrawLine({ size.width, 0 }, size);
}