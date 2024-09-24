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
        static std::vector<size_t> handledIDs;
        static std::vector<ProxyHandler*> aliveProxies;
        static std::deque<ProxyHandler*> cachedProxies;
        static std::vector<ProxyHandler*> pausedProxies;
        static bool paused;

        static void registerProxy(ProxyHandler* proxy);

        GETTER(HttpInfo*, info, Info);
        GETTER(CCHttpRequest*, cocosRequest, CocosRequest);
        GETTER(web::WebRequest*, modRequest, ModRequest);
        GETTER(web::WebTask, modTask, ModTask);
        CCObject* m_originalTarget;
        SEL_HttpResponse m_originalProxy;
        bool m_finished;

        ProxyHandler(CCHttpRequest* request);
        ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url);
        ~ProxyHandler();
        void onCocosResponse(CCHttpClient* client, CCHttpResponse* response);
        void onModResponse(web::WebResponse* response);
        void onResponse();
        web::WebTask::Cancel onCancel();
        void onFinished();
    };
}