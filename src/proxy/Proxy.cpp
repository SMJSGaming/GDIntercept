#include "Proxy.hpp"

using namespace proxy::prelude;
using namespace geode::prelude;

ProxyFilter::ProxyFilter(const enums::EventState state, const enums::OriginFilter origin) :
m_state(state),
m_origin(origin) { };

ProxyFilter::ProxyFilter(const enums::EventState state, std::vector<std::string> urlParts) :
m_state(state),
m_origin(enums::OriginFilter::ALL_FILTER),
m_urlParts(std::move(urlParts)) { };

ProxyFilter::ProxyFilter(cocos2d::CCNode* target, const enums::EventState state, const enums::OriginFilter origin) :
m_state(state),
m_origin(origin) { };

ProxyFilter::ProxyFilter(cocos2d::CCNode* target, const enums::EventState state, std::vector<std::string> urlParts) :
m_state(state),
m_origin(enums::OriginFilter::ALL_FILTER),
m_urlParts(std::move(urlParts)) { };

bool ProxyFilter::validate(const enums::EventState state, std::shared_ptr<HttpInfo> info) const {
    const URL& url = info->getURL();

    return m_state == state &&
        (m_origin == enums::OriginFilter::ALL_FILTER || static_cast<int>(m_origin) == static_cast<int>(url.getOrigin())) &&
        (m_urlParts.empty() || m_urlParts.some([&url](const std::string_view part) {
            return url.getBasicUrl().find(part) != std::string_view::npos;
        }));
}

enums::EventState ProxyFilter::getState() const {
    return m_state;
}

enums::OriginFilter ProxyFilter::getOrigin() const {
    return m_origin;
}

const StringStream& ProxyFilter::getURLParts() const {
    return m_urlParts;
}

ProxyEvent::ProxyEvent(ProxyFilter filter) : m_filter(std::make_shared<ProxyFilter>(std::move(filter))) { }

bool ProxyEvent::send(std::shared_ptr<HttpInfo> info) {
    return ThreadSafeEvent::send(m_filter->getState(), info);
}