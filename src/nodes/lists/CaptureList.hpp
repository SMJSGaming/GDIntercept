#pragma once

#include "../../include.hpp"
#include "../cells/CaptureCell.hpp"

struct CaptureList : public CCNode {
    static CaptureList* create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
private:
    static HttpInfo* active;

    ListView* m_list;

    bool init(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
};