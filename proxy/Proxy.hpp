#pragma once

#include <Geode/Geode.hpp>
#include "HttpInfo.hpp"

namespace proxy {
    using namespace geode::prelude;

    enum SourceFilter {
        GD,
        GD_CDN,
        ROBTOP_GAMES,
        NEWGROUNDS,
        GEODE,
        OTHER,
        ALL
    };

    struct ProxyEvent : public Event {
        ProxyEvent(HttpInfo* info);
        HttpInfo* getRequest() const;
    protected:
        HttpInfo* m_info;
    };

    struct RequestEvent : public ProxyEvent {
        RequestEvent(HttpInfo* info);
    };

    struct ResponseEvent : public ProxyEvent {
        ResponseEvent(HttpInfo* info);
        HttpInfo::HttpContent getResponse(const bool raw = true) const;
    };

    template <typename T>
    concept proxy_event = std::is_base_of_v<ProxyEvent, T>;

    template<proxy_event T>
    struct ProxyFilter : public EventFilter<T> {
        ProxyFilter(const SourceFilter source = ALL) : m_source(source) { };
        ProxyFilter(const std::initializer_list<std::string>& urls) : m_source(ALL), m_urls(urls) { };
        ListenerResult handle(MiniFunction<ListenerResult(T*)> callback, T* event) {
            if (
                (m_urls.empty() || std::find(m_urls.begin(), m_urls.end(), event->getRequest()->getUrl()) != m_urls.end()) &&
                (m_source == ALL || as<int>(m_source) == as<int>(event->getRequest()->getOrigin()))
            ) {
                return callback(event);
            } else {
                return ListenerResult::Propagate;
            }
        }
    protected:
        SourceFilter m_source;
        std::vector<std::string> m_urls;
    };

    struct RequestFilter : public ProxyFilter<RequestEvent> {
        RequestFilter(const SourceFilter source = ALL);
        RequestFilter(const std::initializer_list<std::string>& urls);
    };

    struct ResponseFilter : public ProxyFilter<ResponseEvent> {
        ResponseFilter(const SourceFilter source = ALL);
        ResponseFilter(const std::initializer_list<std::string>& urls);
    };
}