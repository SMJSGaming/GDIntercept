#include "../../proxy/Proxy.hpp"

proxy::ProxyEvent::ProxyEvent(HttpInfo* info) : m_info(info) { }

proxy::HttpInfo* proxy::ProxyEvent::getRequest() const {
    return m_info;
}

proxy::RequestEvent::RequestEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::ResponseEvent::ResponseEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::HttpInfo::content proxy::ResponseEvent::getResponse(const bool raw) const {
    return m_info->getResponseContent(raw);
}

proxy::RequestFilter::RequestFilter(const SourceFilter source) : ProxyFilter(source) { }

proxy::RequestFilter::RequestFilter(const SourceFilter source, const std::initializer_list<std::string>& urls) : ProxyFilter(source, urls) { }

proxy::ResponseFilter::ResponseFilter(const SourceFilter source) : ProxyFilter(source) { }

proxy::ResponseFilter::ResponseFilter(const SourceFilter source, const std::initializer_list<std::string>& urls) : ProxyFilter(source, urls) { }