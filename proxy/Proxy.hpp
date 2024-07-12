#pragma once

#include <Geode/Geode.hpp>
#include "HttpInfo.hpp"

namespace proxy {
    using namespace geode::prelude;

    enum OriginFilter {
        GD,
        GD_CDN,
        ROBTOP_GAMES,
        NEWGROUNDS,
        GEODE,
        LOCALHOST,
        OTHER,
        ALL
    };

    class ProxyEvent : public Event {
    public:
        HttpInfo* getInfo() const;
        HttpInfo::Request getRequest() const;
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
        HttpInfo::Response getResponse() const;
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
            const HttpInfo::URL url = event->getRequest().getURL();
            const std::string raw = url.getQueryLess();

            if (
                (m_urlParts.empty() || std::any_of(m_urlParts.begin(), m_urlParts.end(), [raw](std::string part) {
                    return raw.find(part) != std::string::npos;
                })) &&
                (m_origin == ALL || as<int>(m_origin) == as<int>(url.getOrigin()))
            ) {
                return callback(event);
            } else {
                return ListenerResult::Propagate;
            }
        }
    protected:
        OriginFilter m_origin;
        std::vector<std::string> m_urlParts;

        ProxyFilter(const OriginFilter origin = ALL) : m_origin(origin) { };
        ProxyFilter(const std::vector<std::string>& urlParts) : m_origin(ALL), m_urlParts(urlParts) { };
        ProxyFilter(CCNode* target, const OriginFilter origin = ALL) : m_origin(origin) { };
        ProxyFilter(CCNode* target, const std::vector<std::string>& urlParts) : m_origin(ALL), m_urlParts(urlParts) { };
    };

    class RequestFilter : public ProxyFilter<RequestEvent> {
    public:
        RequestFilter(const OriginFilter origin = ALL);
        RequestFilter(const std::vector<std::string>& urlParts);
        RequestFilter(CCNode* target, const OriginFilter origin = ALL);
        RequestFilter(CCNode* target, const std::vector<std::string>& urlParts);
    };

    class ResponseFilter : public ProxyFilter<ResponseEvent> {
    public:
        ResponseFilter(const OriginFilter origin = ALL);
        ResponseFilter(const std::vector<std::string>& urlParts);
        ResponseFilter(CCNode* target, const OriginFilter origin = ALL);
        ResponseFilter(CCNode* target, const std::vector<std::string>& urlParts);
    };
}