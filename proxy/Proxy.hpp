#pragma once

#include <Geode/Geode.hpp>
#include "HttpInfo.hpp"

namespace proxy {
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

    class ProxyEvent : public geode::Event {
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
    class ProxyFilter : public geode::EventFilter<T> {
    public:
        geode::ListenerResult handle(geode::utils::MiniFunction<geode::ListenerResult(T*)> callback, T* event) {
            const HttpInfo::URL url = event->getRequest().getURL();
            const std::string raw = url.getQueryLess();

            if (
                (m_urlParts.empty() || std::any_of(m_urlParts.begin(), m_urlParts.end(), [raw](std::string part) {
                    return raw.find(part) != std::string::npos;
                })) &&
                (m_origin == ALL || static_cast<int>(m_origin) == static_cast<int>(url.getOrigin()))
            ) {
                return callback(event);
            } else {
                return geode::ListenerResult::Propagate;
            }
        }
    protected:
        OriginFilter m_origin;
        std::vector<std::string> m_urlParts;

        ProxyFilter(const OriginFilter origin = ALL) : m_origin(origin) { };
        ProxyFilter(const std::vector<std::string>& urlParts) : m_origin(ALL), m_urlParts(urlParts) { };
        ProxyFilter(cocos2d::CCNode* target, const OriginFilter origin = ALL) : m_origin(origin) { };
        ProxyFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts) : m_origin(ALL), m_urlParts(urlParts) { };
    };

    class RequestFilter : public ProxyFilter<RequestEvent> {
    public:
        RequestFilter(const OriginFilter origin = ALL);
        RequestFilter(const std::vector<std::string>& urlParts);
        RequestFilter(cocos2d::CCNode* target, const OriginFilter origin = ALL);
        RequestFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts);
    };

    class ResponseFilter : public ProxyFilter<ResponseEvent> {
    public:
        ResponseFilter(const OriginFilter origin = ALL);
        ResponseFilter(const std::vector<std::string>& urlParts);
        ResponseFilter(cocos2d::CCNode* target, const OriginFilter origin = ALL);
        ResponseFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts);
    };
}