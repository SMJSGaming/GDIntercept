#pragma once

#include <Geode/utils/web.hpp>
#include "../include.hpp"
#include "../macro_utils.hpp"

using namespace nlohmann;
using namespace geode::prelude;
using namespace proxy::prelude;

namespace proxy {
    class ProxyHandler : public CCObject {
    public:
        static ProxyHandler* create(CCHttpRequest* request);
        static ProxyHandler* create(web::WebRequest* request, const std::string& method, const std::string& url);
        static std::string getCopyHandshake();
        static std::deque<ProxyHandler*> getFilteredProxies();
        static bool isProxy(CCHttpRequest* request);
        static bool isProxy(web::WebRequest* request);
        static bool isPaused();
        static void pauseAll();
        static void resumeAll();
        static void setCacheLimit(const int64_t limit);
    private:
        static std::vector<size_t> HANDLED_IDS;
        static std::vector<ProxyHandler*> ALIVE_PROXIES;
        static std::deque<ProxyHandler*> CACHED_PROXIES;
        static std::vector<ProxyHandler*> PAUSED_PROXIES;
        static bool PAUSED;

        static void registerProxy(ProxyHandler* proxy);

        GETTER(HttpInfo*, info, Info);
        GETTER(CCHttpRequest*, cocosRequest, CocosRequest);
        GETTER(web::WebRequest*, modRequest, ModRequest);
        GETTER(web::WebTask, modTask, ModTask);
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;
        std::chrono::time_point<std::chrono::steady_clock> m_start;
        bool m_finished;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        ~ProxyHandler();
        size_t calculateResponseTime();
        void onCocosResponse(CCHttpClient* client, CCHttpResponse* response);
        void onModResponse(web::WebResponse* response);
        void onResponse();
        web::WebTask::Cancel onCancel();
        void onFinished();
    };
}