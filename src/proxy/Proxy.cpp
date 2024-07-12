#include "../../proxy/Proxy.hpp"

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

proxy::RequestFilter::RequestFilter(const OriginFilter origin) : ProxyFilter(origin) { }

proxy::RequestFilter::RequestFilter(const std::vector<std::string>& urlParts) : ProxyFilter(urlParts) { }

proxy::RequestFilter::RequestFilter(CCNode* target, const OriginFilter origin) : ProxyFilter(target, origin) { }

proxy::RequestFilter::RequestFilter(CCNode* target, const std::vector<std::string>& urlParts) : ProxyFilter(target, urlParts) { }

proxy::ResponseFilter::ResponseFilter(const OriginFilter origin) : ProxyFilter(origin) { }

proxy::ResponseFilter::ResponseFilter(const std::vector<std::string>& urlParts) : ProxyFilter(urlParts) { }

proxy::ResponseFilter::ResponseFilter(CCNode* target, const OriginFilter origin) : ProxyFilter(target, origin) { }

proxy::ResponseFilter::ResponseFilter(CCNode* target, const std::vector<std::string>& urlParts) : ProxyFilter(target, urlParts) { }