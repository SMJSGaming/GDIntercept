#pragma once

#include "../../include.hpp"

struct CaptureCell : public GenericListCell {
    static CaptureCell* create(HttpInfo* request, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);

    void activate();
    void deactivate();
private:
    HttpInfo* m_request;
    std::function<void(CaptureCell*)> m_switchCell;

    CaptureCell(HttpInfo* request, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);
    bool init(const CCSize& size);
    ccColor3B colorForMethod();
    void onView(CCObject* obj);
};