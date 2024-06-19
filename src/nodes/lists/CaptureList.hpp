#pragma once

#include "../../include.hpp"
#include "../../objects/HttpInfo.hpp"
#include "../cells/CaptureCell.hpp"

struct CaptureList : public CCNode {
    static CaptureList* create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);

    void createCells();
private:
    float m_itemSeparation;
    std::function<void(HttpInfo*)> m_switchInfo;

    CaptureList(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
    void switchCell(CaptureCell* cell);
    bool init() override;
};