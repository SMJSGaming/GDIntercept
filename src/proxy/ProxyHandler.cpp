#include "ProxyHandler.hpp"

std::vector<ProxyHandler*> proxy::ProxyHandler::cachedProxies;

std::vector<ProxyHandler*> ProxyHandler::getProxies() {
    return ProxyHandler::cachedProxies;
}

std::vector<ProxyHandler*> ProxyHandler::getFilteredProxies() {
    const std::string filter(Mod::get()->getSettingValue<std::string>("filter"));
    std::vector<ProxyHandler*> proxies;

    if (ProxyHandler::cachedProxies.empty() || filter == "None") {
        return ProxyHandler::cachedProxies;
    }

    for (ProxyHandler* proxy : ProxyHandler::cachedProxies) {
        switch (proxy->m_info->getRequest()->getURL().getOrigin()) {
            case HttpInfo::Origin::GD: if (filter == "Geometry Dash Server") {
                proxies.push_back(proxy);
            } break;
            case HttpInfo::Origin::GD_CDN: if (filter == "Geometry Dash CDN") {
                proxies.push_back(proxy);
            } break;
            case HttpInfo::Origin::ROBTOP_GAMES: if (filter == "RobtopGames Server") {
                proxies.push_back(proxy);
            } break;
            case HttpInfo::Origin::NEWGROUNDS: if (filter == "Newgrounds CDN") {
                proxies.push_back(proxy);
            } break;
            case HttpInfo::Origin::GEODE: if (filter == "Geode Server") {
                proxies.push_back(proxy);
            } break;
            case HttpInfo::Origin::OTHER: if (filter == "Unknown Origin") {
                proxies.push_back(proxy);
            } break;
        }
    }

    return proxies;
}

ProxyHandler* ProxyHandler::create(CCHttpRequest* request) {
    ProxyHandler* instance = new ProxyHandler(request);

    request->retain();
    instance->retain();
    ProxyHandler::registerProxy(instance);
    RequestEvent(instance->getInfo()).post();

    return instance;
}

ProxyHandler* ProxyHandler::create(web::WebRequest* request, const std::string& method, const std::string& url) {
    ProxyHandler* instance = new ProxyHandler(request, method, url);

    instance->retain();
    ProxyHandler::registerProxy(instance);
    RequestEvent(instance->getInfo()).post();

    return instance;
}

void ProxyHandler::resetCache() {
    for (ProxyHandler* proxy : ProxyHandler::cachedProxies) {
        proxy->getInfo()->resetCache();
    }
}

void ProxyHandler::registerProxy(ProxyHandler* proxy) {
    ProxyHandler::cachedProxies.insert(ProxyHandler::cachedProxies.begin(), proxy);
}

ProxyHandler::ProxyHandler(CCHttpRequest* request) : m_info(new HttpInfo(request)),
m_cocosRequest(request),
m_modRequest(nullptr),
m_originalTarget(request->getTarget()),
m_originalProxy(request->getSelector()) {
    request->setResponseCallback(this, httpresponse_selector(ProxyHandler::onCocosResponse));

    std::thread([this, request]() {
        while (Mod::get()->getSettingValue<bool>("pause-requests")) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        CCHttpClient::getInstance()->send(request);
        m_info->resume();
    }).detach();
}

ProxyHandler::ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url) : m_cocosRequest(nullptr),
m_originalTarget(nullptr),
m_originalProxy(nullptr) {
    m_info = new HttpInfo(m_modRequest = new web::WebRequest(*request), method, url);

    m_modTask = web::WebTask::run([this, method, url](auto progress, auto cancelled) -> web::WebTask::Result {
        web::WebResponse* response = nullptr;

        while (Mod::get()->getSettingValue<bool>("pause-requests")) {
            if (cancelled()) {
                return web::WebTask::Cancel();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        m_modRequest->send(m_info->getRequest()->getURL().getMethod(), url).listen([&response](web::WebResponse* taskResponse) {
            response = new web::WebResponse(*taskResponse);
        }, [&progress](web::WebProgress* taskProgress) {
            progress(*taskProgress);
        });
        m_info->resume();

        while (!response) {
            if (cancelled()) {
                return web::WebTask::Cancel();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        if (cancelled()) {
            return web::WebTask::Cancel();
        } else {
            this->onModResponse(response);

            return *response;
        }
    }, fmt::format("Proxy for {} {}", method, url));
}

void ProxyHandler::onCocosResponse(CCHttpClient* client, CCHttpResponse* response) {
    m_info->m_response = new HttpInfo::Response(m_info->getRequest(), response);

    (m_originalTarget->*m_originalProxy)(client, response);
    ResponseEvent(m_info).post();
}

void ProxyHandler::onModResponse(web::WebResponse* result) {
    m_info->m_response = new HttpInfo::Response(m_info->getRequest(), result);

    ResponseEvent(m_info).post();
}