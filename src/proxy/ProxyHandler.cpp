#include "ProxyHandler.hpp"

std::vector<size_t> proxy::ProxyHandler::HANDLED_IDS;

std::vector<ProxyHandler*> proxy::ProxyHandler::ALIVE_PROXIES;

std::deque<ProxyHandler*> proxy::ProxyHandler::CACHED_PROXIES;

std::vector<ProxyHandler*> proxy::ProxyHandler::PAUSED_PROXIES;

bool proxy::ProxyHandler::PAUSED = Mod::get()->getSettingValue<bool>("confirm-pause-between-plays") && Mod::get()->getSavedValue("paused", false);

ProxyHandler* ProxyHandler::create(CCHttpRequest* request) {
    ProxyHandler* instance = new ProxyHandler(request);

    request->retain();
    instance->retain();
    Loader::get()->queueInMainThread([instance]{ RequestEvent(instance->getInfo()).post(); });

    return instance;
}

ProxyHandler* ProxyHandler::create(web::WebRequest* request, const std::string& method, const std::string& url) {
    ProxyHandler* instance = new ProxyHandler(request, method, url);

    instance->retain();
    Loader::get()->queueInMainThread([instance]{ RequestEvent(instance->getInfo()).post(); });

    return instance;
}

std::string ProxyHandler::getCopyHandshake() {
    return fmt::format("<{}>", "copied"_spr);
}

std::deque<ProxyHandler*> ProxyHandler::getFilteredProxies() {
    const std::string filter(Mod::get()->getSettingValue<std::string>("filter"));
    std::deque<ProxyHandler*> proxies;

    if (ProxyHandler::CACHED_PROXIES.empty() || filter == "None") {
        return ProxyHandler::CACHED_PROXIES;
    }

    for (ProxyHandler* proxy : ProxyHandler::CACHED_PROXIES) {
        switch (proxy->m_info->getRequest().getURL().getOrigin()) {
            case Origin::GD: if (filter == "Geometry Dash Server") {
                proxies.push_back(proxy);
            } break;
            case Origin::GD_CDN: if (filter == "Geometry Dash CDN") {
                proxies.push_back(proxy);
            } break;
            case Origin::ROBTOP_GAMES: if (filter == "RobtopGames Server") {
                proxies.push_back(proxy);
            } break;
            case Origin::NEWGROUNDS: if (filter == "Newgrounds CDN") {
                proxies.push_back(proxy);
            } break;
            case Origin::GEODE: if (filter == "Geode Server") {
                proxies.push_back(proxy);
            } break;
            case Origin::LOCALHOST: if (filter == "Localhost") {
                proxies.push_back(proxy);
            } break;
            case Origin::OTHER: if (filter == "Unknown Origin") {
                proxies.push_back(proxy);
            } break;
        }
    }

    return proxies;
}

bool ProxyHandler::isProxy(CCHttpRequest* request) {
    for (const ProxyHandler* proxy : ProxyHandler::ALIVE_PROXIES) {
        if (proxy->getCocosRequest() == request) {
            return true;
        }
    }

    return false;
}

bool ProxyHandler::isProxy(web::WebRequest* request) {
    return std::find(ProxyHandler::HANDLED_IDS.begin(), ProxyHandler::HANDLED_IDS.end(), request->getID()) != ProxyHandler::HANDLED_IDS.end();
}

bool ProxyHandler::isPaused() {
    return ProxyHandler::PAUSED;
}

void ProxyHandler::pauseAll() {
    Mod::get()->setSavedValue("paused", ProxyHandler::PAUSED = true);
}

void ProxyHandler::resumeAll() {
    const std::vector<ProxyHandler*> paused = std::vector<ProxyHandler*>(ProxyHandler::PAUSED_PROXIES);
    Mod::get()->setSavedValue("paused", ProxyHandler::PAUSED = false);

    std::thread([paused]{
        for (ProxyHandler* proxy : paused) {
            if (proxy->getInfo()->isCancelled()) {
                proxy->onFinished();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                proxy->getInfo()->resume();

                if (CCHttpRequest* cocosRequest = proxy->getCocosRequest()) {
                    Loader::get()->queueInMainThread([proxy, cocosRequest]{
                        proxy->m_start = std::chrono::high_resolution_clock::now();

                        CCHttpClient::getInstance()->send(cocosRequest);
                    });
                }
            }
        }
    }).detach();

    ProxyHandler::PAUSED_PROXIES.clear();
}

void ProxyHandler::setCacheLimit(const int64_t limit) {
    const size_t size = ProxyHandler::CACHED_PROXIES.size();

    if (size > limit) {
        for (size_t i = limit; i < size; i++) {
            ProxyHandler* proxy = ProxyHandler::CACHED_PROXIES.at(i);

            if (proxy->getInfo()->responseReceived()) {
                delete proxy;
            }
        }

        ProxyHandler::CACHED_PROXIES.resize(limit);
    }
}

void ProxyHandler::registerProxy(ProxyHandler* proxy) {
    ProxyHandler::CACHED_PROXIES.push_front(proxy);
    ProxyHandler::ALIVE_PROXIES.push_back(proxy);

    if (proxy->getInfo()->isPaused()) {
        ProxyHandler::PAUSED_PROXIES.push_back(proxy);
    }
    
    if (ProxyHandler::CACHED_PROXIES.size() > Mod::get()->getSettingValue<int64_t>("cache-limit")) {
        ProxyHandler* last = ProxyHandler::CACHED_PROXIES.back();

        ProxyHandler::CACHED_PROXIES.pop_back();

        if (last->m_finished) {
            delete last;
        }
    }
}

ProxyHandler::ProxyHandler(CCHttpRequest* request) : m_modRequest(nullptr),
m_cocosRequest(request),
m_info(new HttpInfo(ProxyHandler::PAUSED, request)),
m_originalTarget(request->getTarget()),
m_originalProxy(request->getSelector()),
m_finished(false) {
    m_cocosRequest->retain();
    m_cocosRequest->setResponseCallback(this, httpresponse_selector(ProxyHandler::onCocosResponse));

    ProxyHandler::registerProxy(this);

    if (!m_info->isPaused()) {
        m_start = std::chrono::high_resolution_clock::now();

        CCHttpClient::getInstance()->send(m_cocosRequest);
    }
}

ProxyHandler::ProxyHandler(web::WebRequest* request, const std::string& method, const std::string& url) : m_modRequest(new web::WebRequest(*request)),
m_cocosRequest(nullptr),
m_info(nullptr),
m_originalTarget(nullptr),
m_originalProxy(nullptr),
m_finished(false) {
    const std::string resendSignature = ProxyHandler::getCopyHandshake();
    const bool isRepeat = m_modRequest->getHeaders().contains(resendSignature);

    m_modRequest->removeHeader(resendSignature);

    m_info = new HttpInfo(ProxyHandler::PAUSED, isRepeat, m_modRequest, method, url);

    ProxyHandler::registerProxy(this);
    ProxyHandler::HANDLED_IDS.push_back(m_modRequest->getID());

    m_modTask = web::WebTask::run([this, url](auto progress, auto cancelled) -> web::WebTask::Result {
        web::WebResponse* response = nullptr;

        while (m_info->isPaused()) {
            ESCAPE_WHEN(cancelled(), this->onCancel());

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        ESCAPE_WHEN(m_info->isCancelled(), this->onCancel());

        m_start = std::chrono::high_resolution_clock::now();
        web::WebTask task = m_modRequest->send(m_info->getRequest().getMethod(), url);

        task.listen([&response](const web::WebResponse* taskResponse) {
            response = new web::WebResponse(*taskResponse);
        }, [progress, cancelled](const web::WebProgress* taskProgress) {
            if (!cancelled()) progress(*taskProgress);
        });

        while (!response && !cancelled() && !m_info->isCancelled()) std::this_thread::sleep_for(std::chrono::milliseconds(2));

        if (m_info->isCancelled() || cancelled()) {
            task.cancel();

            return this->onCancel();
        } else {
            this->onModResponse(response);

            return *response;
        }
    }, fmt::format("Proxy for {} {}", method, url));
}

ProxyHandler::~ProxyHandler() {
    const auto it = std::find(ProxyHandler::ALIVE_PROXIES.begin(), ProxyHandler::ALIVE_PROXIES.end(), this);

    if (it != ProxyHandler::ALIVE_PROXIES.end()) {
        ProxyHandler::ALIVE_PROXIES.erase(it);
    }

    if (m_cocosRequest) {
        m_cocosRequest->release();
        delete m_cocosRequest;
    }

    if (m_modRequest) {
        delete m_modRequest;
    }

    delete m_info;
}

size_t ProxyHandler::calculateResponseTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_start).count();
}

void ProxyHandler::onCocosResponse(CCHttpClient* client, CCHttpResponse* response) {
    m_info->m_response = HttpInfo::Response(&m_info->m_request, response, this->calculateResponseTime());

    (m_originalTarget->*m_originalProxy)(client, response);
    this->onResponse();
}

void ProxyHandler::onModResponse(web::WebResponse* response) {
    m_info->m_response = HttpInfo::Response(&m_info->m_request, response, this->calculateResponseTime());

    this->onResponse();
}

void ProxyHandler::onResponse() {
    Loader::get()->queueInMainThread([this]{
        if (m_info->m_response.m_statusCode < 0) {
            m_info->m_state = State::FAULTY;
        } else {
            m_info->m_state = State::COMPLETED;
        }

        ResponseEvent(m_info).post();
        this->onFinished();
    });
}

web::WebTask::Cancel ProxyHandler::onCancel() {
    const bool shouldPost = m_info->m_state != State::CANCELLED;
    m_info->m_response.m_statusCode = -3;
    m_info->m_state = State::CANCELLED;

    Loader::get()->queueInMainThread([this, shouldPost]{
        if (shouldPost) {
            CancelEvent(m_info).post();
        }

        this->onFinished();
    });

    return web::WebTask::Cancel();
}

void ProxyHandler::onFinished() {
    if (std::find(ProxyHandler::CACHED_PROXIES.begin(), ProxyHandler::CACHED_PROXIES.end(), this) == ProxyHandler::CACHED_PROXIES.end()) {
        delete this;
    } else {
        m_finished = true;
    }
}