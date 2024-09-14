#pragma once

#include <Geode/Geode.hpp>
#include "HttpInfo.hpp"

namespace proxy {
    namespace prelude {
        using namespace proxy;
        using namespace proxy::enums;
        using namespace proxy::converters;
    }

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

    class CancelEvent : public ProxyEvent {
    private:
        CancelEvent(HttpInfo* info);

        friend class HttpInfo;
        friend class ProxyHandler;
    };
        

    template <typename T>
    concept proxy_event = std::is_base_of_v<ProxyEvent, T>;

    template<proxy_event T>
    class ProxyFilter : public geode::EventFilter<T> {
    public:
        geode::ListenerResult handle(geode::utils::MiniFunction<geode::ListenerResult(T*)> callback, T* event) {
            const HttpInfo::URL url = event->getRequest().getURL();

            if ((m_urlParts.empty() || std::any_of(m_urlParts.begin(), m_urlParts.end(), [url](std::string part) {
                return url.getQueryLess().find(part) != std::string::npos;
            })) && (m_origin == enums::OriginFilter::ALL_FILTER || static_cast<int>(m_origin) == static_cast<int>(url.getOrigin()))) {
                return callback(event);
            } else {
                return geode::ListenerResult::Stop;
            }
        }
    protected:
        enums::OriginFilter m_origin;
        std::vector<std::string> m_urlParts;

        ProxyFilter(const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER) : m_origin(origin) { };
        ProxyFilter(const std::vector<std::string>& urlParts) : m_origin(enums::OriginFilter::ALL_FILTER), m_urlParts(urlParts) { };
        ProxyFilter(cocos2d::CCNode* target, const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER) : m_origin(origin) { };
        ProxyFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts) : m_origin(enums::OriginFilter::ALL_FILTER), m_urlParts(urlParts) { };
    };

    class RequestFilter : public ProxyFilter<RequestEvent> {
    public:
        RequestFilter(const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        RequestFilter(const std::vector<std::string>& urlParts);
        RequestFilter(cocos2d::CCNode* target, const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        RequestFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts);
    };

    class ResponseFilter : public ProxyFilter<ResponseEvent> {
    public:
        ResponseFilter(const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        ResponseFilter(const std::vector<std::string>& urlParts);
        ResponseFilter(cocos2d::CCNode* target, const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        ResponseFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts);
    };

    class CancelFilter : public ProxyFilter<CancelEvent> {
    public:
        CancelFilter(const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        CancelFilter(const std::vector<std::string>& urlParts);
        CancelFilter(cocos2d::CCNode* target, const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        CancelFilter(cocos2d::CCNode* target, const std::vector<std::string>& urlParts);
    };
}