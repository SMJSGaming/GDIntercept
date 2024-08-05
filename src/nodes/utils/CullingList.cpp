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
m_cellHeight(cells.empty() ? 0 : cells.at(0)->m_height) { }

CullingList::~CullingList() {
    for (CullingCell* cell : m_cells) {
        delete cell;
    }
}

bool CullingList::init(const CCSize& size) {
    ESCAPE_WHEN(!CCLayerColor::initWithColor(ccc4(0x00, 0x00, 0x00, 0x00), size.width, size.height), false);

    m_view = TableView::create(this, this, nullptr, CCRect(0, 0, size.width, size.height));
    m_view->m_disableHorizontal = true;
    m_view->m_peekLimitTop = 0;
    m_view->m_peekLimitBottom = 0;

    m_view->setMouseEnabled(true);
    this->reloadData();
    this->moveToTop();
    this->addChild(m_view);
    this->scheduleUpdate();

    return true;
}

void CullingList::reloadData() {
    const float oldY = m_view->m_contentLayer->getPositionY();

    m_view->reloadData();
    m_view->m_contentLayer->setContentHeight(m_cellHeight * m_cells.size());
    m_view->m_contentLayer->setPositionY(oldY);

    if ((m_view->m_disableMovement = this->shouldDisableMovement())) {
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
    m_cellHeight = cells.empty() ? 0 : cells.at(0)->m_height;

    m_activeCells.clear();
    this->reloadData();
    this->moveToTop();
}

bool CullingList::shouldDisableMovement() {
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

void CullingList::update(float delta) {
    const size_t size = m_cells.size();

    ESCAPE_WHEN(this->shouldDisableMovement() && m_activeCells.size() == size,);

    const size_t offset = size - std::max<int>(0, std::floor(m_view->m_contentLayer->getPositionY() * -1 / m_cellHeight));
    const size_t amount = std::min<size_t>(std::ceil(this->getContentHeight() / m_cellHeight), size) + 1;
    std::vector<CullingCell*> newActiveCells;

    for (size_t i = 0; i < amount; i++) {
        const size_t index = offset - i - 1;

        CONTINUE_WHEN(index < 0 || index >= size);

        newActiveCells.push_back(this->renderCell(index));
    }

    for (CullingCell* cell : m_activeCells) {
        if (std::find(newActiveCells.begin(), newActiveCells.end(), cell) == newActiveCells.end()) {
            cell->removeFromParent();
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