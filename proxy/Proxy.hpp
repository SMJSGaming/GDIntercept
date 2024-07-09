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

    class ProxyEvent : public Event {
    public:
        HttpInfo::Request* getRequest() const;
    protected:
        HttpInfo* m_info;

        ProxyEvent(HttpInfo* info);
    };

    class RequestEvent : public ProxyEvent {
    private:
        RequestEvent(HttpInfo* info);

        friend class ProxyHandler;
    };

    class ResponseEvent : public ProxyEvent {
    public:
        HttpInfo::Response* getResponse() const;
    private:
        ResponseEvent(HttpInfo* info);

        friend class ProxyHandler;
    };

    template <typename T>
    concept proxy_event = std::is_base_of_v<ProxyEvent, T>;

    template<proxy_event T>
    class ProxyFilter : public EventFilter<T> {
    public:
        ListenerResult handle(MiniFunction<ListenerResult(T*)> callback, T* event) {
            const HttpInfo::URL url = event->getRequest()->getURL();

            if (
                (m_urls.empty() || std::find(m_urls.begin(), m_urls.end(), url.getRaw()) != m_urls.end()) &&
                (m_source == ALL || as<int>(m_source) == as<int>(url.getOrigin()))
            ) {
                return callback(event);
            } else {
                return ListenerResult::Propagate;
            }
        }
    protected:
        SourceFilter m_source;
        std::vector<std::string> m_urls;

        ProxyFilter(const SourceFilter source = ALL) : m_source(source) { };
        ProxyFilter(const std::initializer_list<std::string>& urls) : m_source(ALL), m_urls(urls) { };
    };

    class RequestFilter : public ProxyFilter<RequestEvent> {
    public:
        RequestFilter(const SourceFilter source = ALL);
        RequestFilter(const std::initializer_list<std::string>& urls);
    };

    class ResponseFilter : public ProxyFilter<ResponseEvent> {
    public:
        ResponseFilter(const SourceFilter source = ALL);
        ResponseFilter(const std::initializer_list<std::string>& urls);
    };
}