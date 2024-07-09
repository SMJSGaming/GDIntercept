#pragma once

#include "../../include.hpp"
#include "../cells/CaptureCell.hpp"

class CaptureList : public CCNode {
public:
    static CaptureList* create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
private:
    static HttpInfo* active;

    ListView* m_list;

    bool init(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
};