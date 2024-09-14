#include "../../proxy/Proxy.hpp"

using namespace nlohmann;
using namespace geode::prelude;

proxy::ProxyEvent::ProxyEvent(HttpInfo* info) : m_info(info) { }

proxy::HttpInfo* proxy::ProxyEvent::getInfo() const {
    return m_info;
}

proxy::HttpInfo::Request proxy::ProxyEvent::getRequest() const {
    return m_info->getRequest();
}

proxy::RequestEvent::RequestEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::ResponseEvent::ResponseEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::HttpInfo::Response proxy::ResponseEvent::getResponse() const {
    return m_info->getResponse();
}

proxy::CancelEvent::CancelEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::RequestFilter::RequestFilter(const enums::OriginFilter origin) : ProxyFilter(origin) { }

proxy::RequestFilter::RequestFilter(const std::vector<std::string>& urlParts) : ProxyFilter(urlParts) { }

proxy::RequestFilter::RequestFilter(CCNode* target, const enums::OriginFilter origin) : ProxyFilter(target, origin) { }

proxy::RequestFilter::RequestFilter(CCNode* target, const std::vector<std::string>& urlParts) : ProxyFilter(target, urlParts) { }

proxy::ResponseFilter::ResponseFilter(const enums::OriginFilter origin) : ProxyFilter(origin) { }

proxy::ResponseFilter::ResponseFilter(const std::vector<std::string>& urlParts) : ProxyFilter(urlParts) { }

proxy::ResponseFilter::ResponseFilter(CCNode* target, const enums::OriginFilter origin) : ProxyFilter(target, origin) { }

proxy::ResponseFilter::ResponseFilter(CCNode* target, const std::vector<std::string>& urlParts) : ProxyFilter(target, urlParts) { }

proxy::CancelFilter::CancelFilter(const enums::OriginFilter origin) : ProxyFilter(origin) { }

proxy::CancelFilter::CancelFilter(const std::vector<std::string>& urlParts) : ProxyFilter(urlParts) { }

proxy::CancelFilter::CancelFilter(CCNode* target, const enums::OriginFilter origin) : ProxyFilter(target, origin) { }

proxy::CancelFilter::CancelFilter(CCNode* target, const std::vector<std::string>& urlParts) : ProxyFilter(target, urlParts) { }