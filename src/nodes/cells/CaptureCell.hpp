#pragma once

#include "../../include.hpp"

class CaptureCell : public GenericListCell {
public:
    static CaptureCell* create(const HttpInfo::URL& url, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);

    void activate();
    void deactivate();
private:
    HttpInfo::URL m_url;
    std::function<void(CaptureCell*)> m_switchCell;

    CaptureCell(const HttpInfo::URL& url, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);
    bool init(const CCSize& size);
    ccColor3B colorForMethod();
    void onView(CCObject* obj);
};