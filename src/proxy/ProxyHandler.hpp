#pragma once

#include <Geode/utils/web.hpp>
#include <shared_mutex>
#include "../include.hpp"
#include "../macro_utils.hpp"

using namespace nlohmann;
using namespace geode::prelude;
using namespace proxy::prelude;

namespace proxy {
    class ProxyHandler : private CCObject {
    public:
        static ProxyHandler* create(CCHttpRequest* request);
        static ProxyHandler* create(std::string_view modID, web::WebRequest& request);
        static Stream<ProxyHandler*> getFilteredProxies();
        static bool isProxy(CCHttpRequest* request);
        static bool isProxy(const web::WebRequest& request);
        static bool isPaused();
        static void pauseAll();
        static void resumeAll();
        static void setCacheLimit(const size_t limit);

        ProxyHandler& operator=(const ProxyHandler&) = delete;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(std::string_view modID, web::WebRequest& request);
        ProxyHandler(const ProxyHandler&) = delete;
        ~ProxyHandler();

        std::shared_ptr<HttpInfo> getInfo() noexcept;
    private:
        static Stream<ProxyHandler*> ALIVE_PROXIES;
        static std::shared_mutex ALIVE_MUTEX;
        static std::vector<ProxyHandler*> PAUSED_PROXIES;
        static std::mutex PAUSED_MUTEX;

        static void registerProxy(ProxyHandler* proxy);

        GETTER(std::optional<CCHttpRequest*>, cocosRequest, CocosRequest);
        GETTER(std::optional<web::WebRequest>, modRequest, ModRequest);
        std::shared_ptr<HttpInfo> m_info;
        std::optional<ListenerHandle> m_responseListener;
        std::vector<ListenerHandle> m_proxyListeners;
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;
        bool m_finished;

        void onCocosResponse(CCHttpClient* client, CCHttpResponse* response);
        bool onModResponse(const web::WebResponse& response);
        void onResponse();
    };
}