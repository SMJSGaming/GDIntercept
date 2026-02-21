#pragma once

#include "../include.hpp"
#include "../macro_utils.hpp"
#include "HttpInfo.hpp"
#include "ProxyHandler.hpp"

namespace proxy {
    namespace prelude {
        using namespace proxy;
        using namespace proxy::enums;
        using namespace proxy::converters;
    }

    class ProxyFilter {
    public:
        ProxyFilter(const enums::EventState state, const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        ProxyFilter(const enums::EventState state, std::vector<std::string> urlParts);
        ProxyFilter(cocos2d::CCNode* target, const enums::EventState state, const enums::OriginFilter origin = enums::OriginFilter::ALL_FILTER);
        ProxyFilter(cocos2d::CCNode* target, const enums::EventState state, std::vector<std::string> urlParts);

        bool validate(const enums::EventState state, std::shared_ptr<HttpInfo> info) const;
        enums::EventState getState() const;
        enums::OriginFilter getOrigin() const;
        const StringUtils& getURLParts() const;
    private:
        enums::EventState m_state;
        enums::OriginFilter m_origin;
        StringUtils m_urlParts;
    };

    class ProxyEvent : private geode::ThreadSafeEvent<ProxyEvent, bool(const enums::EventState, std::shared_ptr<HttpInfo>)> {
    public:
        ProxyEvent(ProxyFilter filter);

        bool send(std::shared_ptr<HttpInfo> info);

        template<class Callable>
        geode::comm::ListenerHandle listen(Callable listener, int priority = 0) {
            return ThreadSafeEvent::listen([
                filter = m_filter,
                listener = std::move(listener)
            ](const enums::EventState state, std::shared_ptr<HttpInfo> info) {
                if (filter->validate(state, info)) {
                    listener(info);
                }
            }, priority);
        }
    private:
        std::shared_ptr<ProxyFilter> m_filter;
    };
}