#pragma once

#include <Geode/utils/web.hpp>
#include "../include.hpp"
#include "../macro_utils.hpp"

using namespace nlohmann;
using namespace geode::prelude;

namespace proxy {
    class ProxyHandler : public CCObject {
    public:
        static std::deque<ProxyHandler*> getProxies();
        static std::deque<ProxyHandler*> getFilteredProxies();
        static std::deque<ProxyHandler*> getAliveProxies();
        static ProxyHandler* create(CCHttpRequest* request);
        static ProxyHandler* create(web::WebRequest* request, const std::string& method, const std::string& url);
        static void setCacheLimit(const int64_t limit);
        static void resumeAll();
    private:
        static std::deque<ProxyHandler*> aliveProxies;
        static std::deque<ProxyHandler*> cachedProxies;
        static std::vector<ProxyHandler*> pausedProxies;

        static void registerProxy(ProxyHandler* proxy);

        GETTER(HttpInfo*, info, Info);
        GETTER(CCHttpRequest*, cocosRequest, CocosRequest);
        GETTER(web::WebRequest*, modRequest, ModRequest);
        GETTER(web::WebTask, modTask, ModTask);
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        ~ProxyHandler();
        void onCocosResponse(CCHttpClient* client, CCHttpResponse* response);
        void onModResponse(web::WebResponse* response);
        void onResponse();
    };
}