#include "ProxyHandler.hpp"

std::vector<ProxyHandler*> proxy::ProxyHandler::cachedProxies;

std::vector<ProxyHandler*> ProxyHandler::getProxies() {
    return ProxyHandler::cachedProxies;
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
        web::WebResponse response;

        while (Mod::get()->getSettingValue<bool>("pause-requests")) {
            if (cancelled()) {
                return web::WebTask::Cancel();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        m_modRequest->send(m_info->getMethod(), url).listen([&response](web::WebResponse* taskResponse) {
            response = web::WebResponse(*taskResponse);
        }, [&progress](web::WebProgress* taskProgress) {
            progress(*taskProgress);
        });
        m_info->resume();

        while (!response.code()) {
            if (cancelled()) {
                return web::WebTask::Cancel();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        if (cancelled()) {
            return web::WebTask::Cancel();
        } else {
            this->onModResponse(response);

            return response;
        }
    }, fmt::format("Proxy for {} {}", method, url));
}

void ProxyHandler::onCocosResponse(CCHttpClient* client, CCHttpResponse* response) {
    gd::vector<char>* data = response->getResponseData();

    m_info->m_statusCode = response->getResponseCode();
    m_info->m_responseContentType = m_info->determineContentType(m_info->m_response = std::string(data->begin(), data->end()));

    (m_originalTarget->*m_originalProxy)(client, response);
    ResponseEvent(m_info).post();
}

void ProxyHandler::onModResponse(web::WebResponse result) {
    m_info->m_statusCode = result.code();
    m_info->m_responseContentType = m_info->determineContentType(m_info->m_response = result.string().unwrapOrDefault());

    ResponseEvent(m_info).post();
}