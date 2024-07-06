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
        web::WebRequest* getModRequest();
        web::WebTask getModTask();
    private:
        static std::vector<ProxyHandler*> cachedProxies;

        static void registerProxy(ProxyHandler* proxy);

        HttpInfo* m_info;
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;
        CCHttpRequest* m_cocosRequest;
        web::WebRequest* m_modRequest;
        web::WebTask m_modTask;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        void onCocosResponse(CCHttpClient* client, CCHttpResponse* response);
        void onModResponse(web::WebResponse* result);
    };
}