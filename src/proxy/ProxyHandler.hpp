#pragma once

#include <Geode/utils/web.hpp>
#include "../include.hpp"

#define GETTER(type, name, capital_name) \
        public: \
            type get##capital_name() const { return m_##name; } \
        private: \
            type m_##name;

namespace proxy {
    class ProxyHandler : public CCObject {
    public:
        static std::vector<ProxyHandler*> getProxies();
        static std::vector<ProxyHandler*> getFilteredProxies();
        static ProxyHandler* create(CCHttpRequest* request);
        static ProxyHandler* create(web::WebRequest* request, const std::string& method, const std::string& url);
        static void resetCache();
        static void resumeAll();
    private:
        static std::vector<ProxyHandler*> cachedProxies;

        static void registerProxy(ProxyHandler* proxy);

        GETTER(HttpInfo*, info, Info)
        GETTER(CCHttpRequest*, cocosRequest, CocosRequest)
        GETTER(web::WebRequest*, modRequest, ModRequest)
        GETTER(web::WebTask, modTask, ModTask)
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        void onCocosResponse(CCHttpClient* client, CCHttpResponse* response);
        void onModResponse(web::WebResponse* result);
    };
}