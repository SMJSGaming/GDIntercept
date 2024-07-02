#include "ProxyHandler.hpp"

ProxyHandler* ProxyHandler::create(CCHttpRequest* request) {
    ProxyHandler* instance = new ProxyHandler(request);

    instance->retain();
    RequestEvent(instance->getInfo()).post();

    return instance;
}

ProxyHandler* ProxyHandler::create(web::WebRequest* request, const std::string& method, const std::string& url) {
    ProxyHandler* instance = new ProxyHandler(request, method, url);

    instance->retain();
    RequestEvent(instance->getInfo()).post();

    return instance;
}

ProxyHandler::ProxyHandler(CCHttpRequest* request) : m_info(new HttpInfo(request)),
    m_originalTarget(request->getTarget()),
    m_originalProxy(request->getSelector()) {
    request->setResponseCallback(this, httpresponse_selector(ProxyHandler::onResponse));
}

ProxyHandler::~ProxyHandler() {
    delete m_info;
}

ProxyHandler::ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url) : m_info(new HttpInfo(request, method, url)),
    m_originalTarget(nullptr),
    m_originalProxy(nullptr) {
    // request->setResponseCallback(this, httpresponse_selector(ProxyHandler::onResponse));
}

HttpInfo* ProxyHandler::getInfo() {
    return m_info;
}

void ProxyHandler::onResponse(CCHttpClient* client, CCHttpResponse* response) {
    gd::vector<char>* data = response->getResponseData();

    m_info->m_response = std::string(data->begin(), data->end());
    m_info->m_statusCode = response->getResponseCode();
    m_info->m_responseContentType = m_info->determineContentType(m_info->m_response);

    (m_originalTarget->*m_originalProxy)(client, response);
    ResponseEvent(m_info).post();
}