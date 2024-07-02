#pragma once

#include <Geode/utils/web.hpp>
#include "../include.hpp"

namespace proxy {
    struct ProxyHandler : public CCObject {
        static std::vector<ProxyHandler*> getProxies();
        static ProxyHandler* create(CCHttpRequest* request);
        static ProxyHandler* create(web::WebRequest* request, const std::string& method, const std::string& url);
        static void resumeRequests();
        static void forgetProxies();
        static void resetCache();

        ~ProxyHandler();
        HttpInfo* getInfo();
        CCHttpRequest* getCocosRequest();
    private:
        static std::vector<ProxyHandler*> cachedProxies;

        static void registerProxy(ProxyHandler* proxy);

        HttpInfo* m_info;
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;
        CCHttpRequest* m_cocosRequest;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        void onResponse(CCHttpClient* client, CCHttpResponse* response);
    };
}