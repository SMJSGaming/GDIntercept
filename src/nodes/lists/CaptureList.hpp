#pragma once

#include "../../include.hpp"
#include "TouchFixList.hpp"
#include "../cells/CaptureCell.hpp"
#include "../../objects/HttpInfo.hpp"

struct CaptureList : public CCNode {
    static CaptureList* create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
private:
    TouchFixList* m_list;

    bool init(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo);
};