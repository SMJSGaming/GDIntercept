#include "CullingList.hpp"

CullingList* CullingList::create(const CCSize& size, const std::vector<CullingCell*>& cells) {
    CullingList* instance = new CullingList(cells);

    if (instance && instance->init(size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CullingList::CullingList(const std::vector<CullingCell*>& cells) : m_cells(cells),
m_cellHeight(cells.empty() ? 0 : cells.at(0)->m_height),
m_view(nullptr) { }

CullingList::~CullingList() {
    for (CullingCell* cell : m_cells) {
        cell->removeFromParent();

        delete cell;
    }
}

bool CullingList::init(const CCSize& size) {
    ESCAPE_WHEN(!CCLayerColor::initWithColor(ccc4(0x00, 0x00, 0x00, 0x00), size.width, size.height), false);

    m_view = TableView::create(this, this, nullptr, CCRect(0, 0, size.width, size.height));
    m_view->m_peekLimitTop = 0;
    m_view->m_peekLimitBottom = 0;

    this->reloadData();
    this->moveToTop();
    this->addChild(m_view);
    this->scheduleUpdate();

    return true;
}

void CullingList::reloadData() {
    m_cellHeight = m_cells.empty() ? 0 : m_cells.at(0)->m_height;
    m_maxCellWidth = 0;
    m_scrollLockSteps = 0;
    m_lastOffset = ccp(0, 0);

    m_view->reloadData();

    for (CullingCell* cell : m_cells) {
        m_maxCellWidth = std::max(m_maxCellWidth, cell->m_width);
    }

    m_view->m_contentLayer->setPositionX(0);
    m_view->m_contentLayer->setContentSize({ m_maxCellWidth, m_cellHeight * m_cells.size() });
    m_view->m_disableHorizontal = m_maxCellWidth <= m_view->getContentWidth();
    m_view->m_disableVertical = this->shouldDisableVertical();
    // For some reason if m_view->m_disableVertical == false, m_view->m_disableHorizontal is ignored and rather relies on m_view->m_disableMovement
    m_view->m_disableMovement = m_view->m_disableHorizontal && m_view->m_disableVertical;

    if (m_view->m_disableVertical) {
        m_view->m_contentLayer->setContentHeight(m_view->getContentHeight());
    }
}

void CullingList::moveToTop() {
    m_view->m_contentLayer->setPositionY(m_view->getContentHeight() - m_view->m_contentLayer->getContentHeight());
}

void CullingList::moveToBottom() {
    m_view->m_contentLayer->setPositionY(0);
}

void CullingList::setCells(const std::vector<CullingCell*>& cells) {
    for (CullingCell* cell : m_cells) {
        cell->removeFromParent();

        delete cell;
    }

    m_cells = cells;

    m_activeCells.clear();
    this->reloadData();
    this->moveToTop();
}

bool CullingList::isHorizontalLocked() {
    return m_view->m_disableHorizontal;
}

bool CullingList::shouldDisableVertical() {
    return m_view->m_contentLayer->getContentHeight() <= m_view->getContentHeight();
}

CullingCell* CullingList::renderCell(const size_t index) {
    CullingCell* cell = m_cells.at(index);

    if (!cell->isActive()) {
        m_view->m_contentLayer->addChild(cell);
        cell->render();
        cell->setPositionY(m_view->m_contentLayer->getContentHeight() - (index + 1) * m_cellHeight);

        cell->m_indexPath.m_row = index;
        cell->m_tableView = m_view;
    }

    return cell;
}

void CullingList::horizontalRender(CullingCell* cell) {
    CCNode* node = cell->getChildByIDRecursive("horizontal-align");
    const float x = -m_view->m_contentLayer->getPositionX();

    if (node != nullptr) {
        node->setPositionX(x);
    }
}

void CullingList::setContentWidth(const float width) {
    CCLayerColor::setContentWidth(width);

    if (m_view != nullptr) {
        m_view->setContentHeight(width);
        this->reloadData();
    }
}

void CullingList::setContentHeight(const float height) {
    CCLayerColor::setContentHeight(height);

    if (m_view != nullptr) {
        m_view->setContentHeight(height);
        this->reloadData();
    }
}

void CullingList::setContentSize(const CCSize& size) {
    CCLayerColor::setContentSize(size);

    if (m_view != nullptr) {
        m_view->setContentSize(size);
        this->reloadData();
    }
}

void CullingList::update(const float dt) {
    const size_t size = m_cells.size();
    const CCPoint contentOffset = m_view->m_contentLayer->getPosition();
    const bool moved = contentOffset != m_lastOffset;
    const bool shouldSkipCulling = m_view->m_disableVertical && m_activeCells.size() == size;

    // Alignment snapping
    if (moved) {
        if (m_scrollLockSteps == 0) {
            m_xAxisLocked = std::abs(contentOffset.x - m_lastOffset.x) > std::abs(contentOffset.y - m_lastOffset.y);
            m_scrollLockSteps = Mod::get()->getSettingValue<int64_t>("snapping-aggression");
        }

        if (m_xAxisLocked) {
            m_lastOffset.x = contentOffset.x;

            m_view->m_contentLayer->setPositionY(m_lastOffset.y);
        } else {
            m_lastOffset.y = contentOffset.y;

            m_view->m_contentLayer->setPositionX(m_lastOffset.x);
        }
    }

    if (m_scrollLockSteps > 0) {
        m_scrollLockSteps--;
    }

    m_view->m_contentLayer->setPositionX(std::min(0.0f, std::max(m_view->getContentWidth() - m_maxCellWidth, m_lastOffset.x)));

    for (CullingCell* cell : m_activeCells) {
        this->horizontalRender(cell);
    }

    ESCAPE_WHEN(m_activeCells.size() && (!moved || shouldSkipCulling),);

    const size_t offset = size - std::max<int>(0, std::floor(-m_lastOffset.y / m_cellHeight));
    const size_t amount = std::min<size_t>(std::ceil(this->getContentHeight() / m_cellHeight), size) + 1;
    std::vector<CullingCell*> newActiveCells;

    for (size_t i = 0; i < amount; i++) {
        const size_t index = offset - i - 1;

        CONTINUE_WHEN(index < 0 || index >= size);

        CullingCell* cell = this->renderCell(index);

        this->horizontalRender(cell);
        newActiveCells.push_back(cell);
    }

    for (CullingCell* cell : m_activeCells) {
        if (std::find(newActiveCells.begin(), newActiveCells.end(), cell) == newActiveCells.end()) {
            cell->removeFromParentAndCleanup(false);
            cell->discard();
        }
    }

    m_activeCells = newActiveCells;
}

int CullingList::numberOfRowsInSection(unsigned int section, TableView* view) {
    return 0;
}

unsigned int CullingList::numberOfSectionsInTableView(TableView* view) {
    return 0;
}

float CullingList::cellHeightForRowAtIndexPath(CCIndexPath& indexPath, TableView* view) {
    return m_cellHeight;
}

TableViewCell* CullingList::cellForRowAtIndexPath(CCIndexPath& indexPath, TableView* view) {
    return nullptr;
}