#pragma once

#include "../../../include.hpp"
#include "../cells/CullingCell.hpp"

class CullingList : public CCLayerColor, public TableViewDelegate, public TableViewDataSource {
public:
    static CullingList* create(const CCSize& size, const std::vector<CullingCell*>& cells = {});

    ~CullingList();
    void reloadData();
    void moveToTop();
    void moveToBottom();
    void toggleCulling(const bool enabled);
    void setCells(const std::vector<CullingCell*>& cells);
    void setContentWidth(const float width);
    void setContentHeight(const float height);
    void setContentSize(const CCSize& size) override;
    bool isHorizontalLocked();
protected:
    GETTER(std::vector<CullingCell*>, cells, Cells);
    PROTECTED_GETTER(std::vector<CullingCell*>, activeCells, ActiveCells);
    PROTECTED_GETTER(float, cellHeight, CellHeight);
    PROTECTED_GETTER(float, maxCellWidth, MaxCellWidth);
    PROTECTED_GETTER(TableView*, view, View);
    CCPoint m_lastOffset;
    size_t m_scrollLockSteps;
    bool m_xAxisLocked;
    bool m_disableCulling;

    CullingList(const std::vector<CullingCell*>& cells);
    bool init(const CCSize& size);
    bool shouldDisableVertical();
    void horizontalRender(CullingCell* cell);
    CullingCell* renderCell(const size_t index);
    void update(const float dt) override;
    int numberOfRowsInSection(unsigned int section, TableView* view) override;
    unsigned int numberOfSectionsInTableView(TableView* view) override;
    float cellHeightForRowAtIndexPath(CCIndexPath& indexPath, TableView* view) override;
    TableViewCell* cellForRowAtIndexPath(CCIndexPath& indexPath, TableView* view) override;
};