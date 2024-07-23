#include "ProxyHandler.hpp"

std::deque<ProxyHandler*> proxy::ProxyHandler::aliveProxies;

std::deque<ProxyHandler*> proxy::ProxyHandler::cachedProxies;

std::vector<ProxyHandler*> proxy::ProxyHandler::pausedProxies;

ProxyHandler* ProxyHandler::create(CCHttpRequest* request) {
    ProxyHandler* instance = new ProxyHandler(request);

    request->retain();
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

std::deque<ProxyHandler*> ProxyHandler::getProxies() {
    return ProxyHandler::cachedProxies;
}

std::deque<ProxyHandler*> ProxyHandler::getAliveProxies() {
    return ProxyHandler::aliveProxies;
}

std::deque<ProxyHandler*> ProxyHandler::getFilteredProxies() {
    const std::string filter(Mod::get()->getSettingValue<std::string>("filter"));
    std::deque<ProxyHandler*> proxies;

    if (ProxyHandler::cachedProxies.empty() || filter == "None") {
        return ProxyHandler::cachedProxies;
    }

    for (ProxyHandler* proxy : ProxyHandler::cachedProxies) {
        switch (proxy->m_info->getRequest().getURL().getOrigin()) {
            case HttpInfo::Origin::GD: CASE_BREAK(if (filter == "Geometry Dash Server") {
                proxies.push_back(proxy);
            });
            case HttpInfo::Origin::GD_CDN: CASE_BREAK(if (filter == "Geometry Dash CDN") {
                proxies.push_back(proxy);
            });
            case HttpInfo::Origin::ROBTOP_GAMES: CASE_BREAK(if (filter == "RobtopGames Server") {
                proxies.push_back(proxy);
            });
            case HttpInfo::Origin::NEWGROUNDS: CASE_BREAK(if (filter == "Newgrounds CDN") {
                proxies.push_back(proxy);
            });
            case HttpInfo::Origin::GEODE: CASE_BREAK(if (filter == "Geode Server") {
                proxies.push_back(proxy);
            });
            case HttpInfo::LOCALHOST: CASE_BREAK(if (filter == "Localhost") {
                proxies.push_back(proxy);
            });
            case HttpInfo::Origin::OTHER: CASE_BREAK(if (filter == "Unknown Origin") {
                proxies.push_back(proxy);
            });
        }
    }

    return proxies;
}

void ProxyHandler::setCacheLimit(const int64_t limit) {
    const size_t size = ProxyHandler::cachedProxies.size();

    if (size > limit) {
        for (size_t i = limit; i < size; i++) {
            ProxyHandler* proxy = ProxyHandler::cachedProxies.at(i);

            if (proxy->getInfo()->responseReceived()) {
                delete proxy;
            }
        }

        ProxyHandler::cachedProxies.resize(limit);
    }
}

void ProxyHandler::resumeAll() {
    const std::vector<ProxyHandler*> paused = std::vector<ProxyHandler*>(ProxyHandler::pausedProxies);

    std::thread([paused]{
        for (ProxyHandler* proxy : paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            proxy->getInfo()->resume();

            if (CCHttpRequest* cocosRequest = proxy->getCocosRequest()) {
                Loader::get()->queueInMainThread([cocosRequest]{ CCHttpClient::getInstance()->send(cocosRequest); });
            }
        }
    }).detach();

    ProxyHandler::pausedProxies.clear();
}

void ProxyHandler::registerProxy(ProxyHandler* proxy) {
    ProxyHandler::cachedProxies.push_front(proxy);
    ProxyHandler::aliveProxies.push_back(proxy);

    if (proxy->getInfo()->isPaused()) {
        ProxyHandler::pausedProxies.push_back(proxy);
    }
    
    if (ProxyHandler::cachedProxies.size() > Mod::get()->getSettingValue<int64_t>("cache-limit")) {
        ProxyHandler* last = ProxyHandler::cachedProxies.back();

        ProxyHandler::cachedProxies.pop_back();

        if (last->getInfo()->responseReceived()) {
            delete last;
        }
    }
}

ProxyHandler::ProxyHandler(CCHttpRequest* request) : m_modRequest(nullptr),
m_cocosRequest(request),
m_info(new HttpInfo(request)),
m_originalTarget(request->getTarget()),
m_originalProxy(request->getSelector()) {
    m_cocosRequest->retain();
    m_cocosRequest->setResponseCallback(this, httpresponse_selector(ProxyHandler::onCocosResponse));

    ProxyHandler::registerProxy(this);

    if (!m_info->isPaused()) {
        CCHttpClient::getInstance()->send(m_cocosRequest);
    }
}

ProxyHandler::ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url) : m_modRequest(new web::WebRequest(*request)),
m_cocosRequest(nullptr),
m_info(nullptr),
m_originalTarget(nullptr),
m_originalProxy(nullptr) {
    m_info = new HttpInfo(m_modRequest, method, url);

    ProxyHandler::registerProxy(this);

    m_modTask = web::WebTask::run([this, method, url](auto progress, auto cancelled) -> web::WebTask::Result {
        web::WebResponse* response = nullptr;

        while (m_info->isPaused()) {
            if (cancelled()) {
                m_info->m_response.m_statusCode = -3;

                return web::WebTask::Cancel();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        web::WebTask task = m_modRequest->send(m_info->getRequest().getURL().getMethod(), url);

        task.listen([&response](web::WebResponse* taskResponse) {
            response = new web::WebResponse(*taskResponse);
        }, [progress](web::WebProgress* taskProgress) {
            progress(*taskProgress);
        });

        while (!response) {
            BREAK_WHEN(cancelled());

            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        if (cancelled()) {
            task.cancel();
            m_info->m_response.m_statusCode = -3;

            return web::WebTask::Cancel();
        } else {
            this->onModResponse(response);

            return *response;
        }
    }, fmt::format("Proxy for {} {}", method, url));
}

ProxyHandler::~ProxyHandler() {
    ProxyHandler::aliveProxies.erase(std::find(ProxyHandler::aliveProxies.begin(), ProxyHandler::aliveProxies.end(), this));

    if (m_cocosRequest) {
        delete m_cocosRequest;
    }

    if (m_modRequest) {
        delete m_modRequest;
    }

    delete m_info;
}

void ProxyHandler::onCocosResponse(CCHttpClient* client, CCHttpResponse* response) {
    m_info->m_response = HttpInfo::Response(&m_info->m_request, response);

    (m_originalTarget->*m_originalProxy)(client, response);
    this->onResponse();
}

void ProxyHandler::onModResponse(web::WebResponse* response) {
    m_info->m_response = HttpInfo::Response(&m_info->m_request, response);

    this->onResponse();
}

void ProxyHandler::onResponse() {
    ResponseEvent(m_info).post();

    if (std::find(ProxyHandler::cachedProxies.begin(), ProxyHandler::cachedProxies.end(), this) == ProxyHandler::cachedProxies.end()) {
        delete this;
    }
}