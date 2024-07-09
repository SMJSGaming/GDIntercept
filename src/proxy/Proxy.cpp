#include "../../proxy/Proxy.hpp"

proxy::ProxyEvent::ProxyEvent(HttpInfo* info) : m_info(info) { }

proxy::HttpInfo::Request proxy::ProxyEvent::getRequest() const {
    return m_info->getRequest();
}

proxy::RequestEvent::RequestEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::ResponseEvent::ResponseEvent(HttpInfo* info) : ProxyEvent(info) { }

proxy::HttpInfo::Response proxy::ResponseEvent::getResponse() const {
    return m_info->getResponse();
}

proxy::RequestFilter::RequestFilter(const SourceFilter source) : ProxyFilter(source) { }

proxy::RequestFilter::RequestFilter(const std::initializer_list<std::string>& urls) : ProxyFilter(urls) { }

proxy::RequestFilter::RequestFilter(CCNode* target, const SourceFilter source) : ProxyFilter(target, source) { }

proxy::RequestFilter::RequestFilter(CCNode* target, const std::initializer_list<std::string>& urls) : ProxyFilter(target, urls) { }

proxy::ResponseFilter::ResponseFilter(const SourceFilter source) : ProxyFilter(source) { }

proxy::ResponseFilter::ResponseFilter(const std::initializer_list<std::string>& urls) : ProxyFilter(urls) { }

proxy::ResponseFilter::ResponseFilter(CCNode* target, const SourceFilter source) : ProxyFilter(target, source) { }

proxy::ResponseFilter::ResponseFilter(CCNode* target, const std::initializer_list<std::string>& urls) : ProxyFilter(target, urls) { }