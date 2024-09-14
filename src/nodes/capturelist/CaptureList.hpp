#pragma once

#include "../../include.hpp"
#include "cells/CaptureCell.hpp"
#include "../utils/KeybindNode.hpp"

class CaptureList : public KeybindNode<Border> {
public:
    static CaptureList* create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
private:
    static size_t active;

    CCArrayExt<CaptureCell*> m_cells;

    void setup() override;
    bool init(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
    void tabCell(const bool forward);
};