#pragma once

#include "../../../include.hpp"
#include "../utils/Badge.hpp"
#include "../../utils/Character.hpp"
#include "../../../proxy/Proxy.hpp"
#include "../../utils/RescalingNode.hpp"

class CaptureCell : public GenericListCell {
public:
    static CaptureCell* create(const size_t index, std::shared_ptr<proxy::HttpInfo> info, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);

    void activate();
    void deactivate();
private:
    template<typename T>
    static std::tuple<std::string, CCNode*, std::function<void(GLubyte)>> makeBadgeInfo(std::string name, T* node);

    PRIMITIVE_GETTER(std::shared_ptr<proxy::HttpInfo>, info, Info);
    std::function<void(CaptureCell*)> m_switchCell;

    CaptureCell(std::shared_ptr<proxy::HttpInfo> info, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell);
    bool init(const size_t index, const CCSize& size);
    std::vector<std::tuple<std::string, CCNode*, std::function<void(GLubyte)>>> badgesForRequest();
    ccColor3B colorForMethod();
    void onView(CCObject* obj);
};