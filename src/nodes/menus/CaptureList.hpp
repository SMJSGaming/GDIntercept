#pragma once

#include "../../include.hpp"
#include "../cells/CaptureCell.hpp"

class CaptureList : public Border {
public:
    static CaptureList* create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
private:
    static size_t active;

    CCArrayExt<CaptureCell*> m_cells;

    bool init(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
    void tabCell(const bool forward);
};