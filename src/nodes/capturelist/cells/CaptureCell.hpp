#pragma once

#include "../../../include.hpp"
#include "../utils/Badge.hpp"
#include "../../utils/Character.hpp"
#include "../../utils/RescalingNode.hpp"

class CaptureCell : public GenericListCell {
public:
    static CaptureCell* create(const size_t index, const HttpInfo* info, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);

    void activate();
    void deactivate();
private:
    template<typename T>
    static std::tuple<std::string, CCNode*, std::function<void(GLubyte)>> makeBadgeInfo(const std::string& name, T* node);

    const HttpInfo* m_info;
    std::function<void(CaptureCell*)> m_switchCell;

    CaptureCell(const HttpInfo* info, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);
    bool init(const size_t index, const CCSize& size);
    std::vector<std::tuple<std::string, CCNode*, std::function<void(GLubyte)>>> badgesForRequest();
    ccColor3B colorForMethod();
    void onView(CCObject* obj);
};