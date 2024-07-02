#pragma once

#include <Geode/utils/web.hpp>
#include "../include.hpp"

namespace proxy {
    struct ProxyHandler : public CCObject {
        static ProxyHandler* create(CCHttpRequest* request);
        static ProxyHandler* create(web::WebRequest* request, const std::string& method, const std::string& url);

        HttpInfo* getInfo();
        ~ProxyHandler();
    private:
        HttpInfo* m_info;
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        void onResponse(CCHttpClient* client, CCHttpResponse* response);
    };
}