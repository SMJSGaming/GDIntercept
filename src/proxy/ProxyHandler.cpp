#include "ProxyHandler.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    Mod::get()->setSavedValue("paused", Mod::get()->getSettingValue<bool>("pause-between-plays") && Mod::get()->getSavedValue("paused", false));

    web::WebRequestInterceptEvent().listen([](std::string_view modID, web::WebRequest& request) {
        ProxyHandler::create(modID, request);

        return ListenerResult::Propagate;
    }, 1'000'000).leak();
}

Stream<ProxyHandler*> proxy::ProxyHandler::ALIVE_PROXIES;

std::shared_mutex proxy::ProxyHandler::ALIVE_MUTEX;

std::vector<ProxyHandler*> proxy::ProxyHandler::PAUSED_PROXIES;

std::mutex proxy::ProxyHandler::PAUSED_MUTEX;

ProxyHandler* ProxyHandler::create(CCHttpRequest* request) {
    ProxyHandler* proxy = new ProxyHandler(request);

    ProxyHandler::registerProxy(proxy);

    return proxy;
}

ProxyHandler* ProxyHandler::create(std::string_view modID, web::WebRequest& request) {
    ProxyHandler* proxy = new ProxyHandler(modID, request);

    ProxyHandler::registerProxy(proxy);

    return proxy;
}

Stream<ProxyHandler*> ProxyHandler::getFilteredProxies() {
    std::shared_lock aliveLock(ProxyHandler::ALIVE_MUTEX);

    static const std::unordered_map<Origin, std::string> proxyTranslations = {
        { Origin::GD, "Geometry Dash Server" },
        { Origin::GD_CDN, "Geometry Dash CDN" },
        { Origin::ROBTOP_GAMES, "RobtopGames Server" },
        { Origin::NEWGROUNDS, "Newgrounds CDN" },
        { Origin::GEODE, "Geode Server" },
        { Origin::LOCALHOST, "Localhost" },
        { Origin::OTHER, "Unknown Origin" }
    };
    const std::string filter = Mod::get()->getSettingValue<std::string>("filter");

    if (ProxyHandler::ALIVE_PROXIES.empty() || filter == "None") {
        return Stream(ProxyHandler::ALIVE_PROXIES.begin(), ProxyHandler::ALIVE_PROXIES.end());
    } else {
        return Stream(ProxyHandler::ALIVE_PROXIES.begin(), ProxyHandler::ALIVE_PROXIES.end()).filter([&filter](ProxyHandler* proxy) {
            return proxyTranslations.at(proxy->m_info->getRequest().getURL().getOrigin()) == filter;
        });
    }
}

bool ProxyHandler::isProxy(CCHttpRequest* request) {
    std::shared_lock aliveLock(ProxyHandler::ALIVE_MUTEX);

    return ProxyHandler::ALIVE_PROXIES.some([request](ProxyHandler* proxy) {
        return proxy->getCocosRequest() == request;
    });
}

bool ProxyHandler::isProxy(const web::WebRequest& request) {
    std::shared_lock aliveLock(ProxyHandler::ALIVE_MUTEX);

    return ProxyHandler::ALIVE_PROXIES.some([request](ProxyHandler* proxy) {
        const std::optional<web::WebRequest>& proxyReq = proxy->getModRequest();

        return proxyReq.has_value() && proxyReq.value().getID() == request.getID();
    });
}

bool ProxyHandler::isPaused() {
    return Mod::get()->getSavedValue<bool>("paused");
}

void ProxyHandler::pauseAll() {
    Mod::get()->setSavedValue("paused", true);
}

// Thread unsafe
void ProxyHandler::resumeAll() {
    Mod::get()->setSavedValue("paused", false);
    std::vector<ProxyHandler*> paused;

    {
        std::lock_guard pauseLock(ProxyHandler::PAUSED_MUTEX);

        paused.swap(ProxyHandler::PAUSED_PROXIES);
    }

    for (ProxyHandler* proxy : paused) {
        const std::optional<CCHttpRequest*>& request = proxy->getCocosRequest();

        if (!proxy->m_finished && request.has_value()) {
            proxy->m_start = std::chrono::high_resolution_clock::now();

            proxy->m_info->resume();
            CCHttpClient::getInstance()->send(request.value());
        }
    }
}

void ProxyHandler::setCacheLimit(const size_t limit) {
    Stream<ProxyHandler*> removedHandlers;

    {
        std::unique_lock aliveLock(ProxyHandler::ALIVE_MUTEX);

        if (ProxyHandler::ALIVE_PROXIES.size() > limit) {
            removedHandlers = ProxyHandler::ALIVE_PROXIES.slice(limit);

            ProxyHandler::ALIVE_PROXIES.resize(limit);
        }
    }

    removedHandlers.forEach([](ProxyHandler* handler) { delete handler; });
}

void ProxyHandler::registerProxy(ProxyHandler* proxy) {
    {
        std::unique_lock aliveLock(ProxyHandler::ALIVE_MUTEX);
        ProxyHandler::ALIVE_PROXIES = Stream({ proxy }).concat(ProxyHandler::ALIVE_PROXIES);

        if (ProxyHandler::ALIVE_PROXIES.size() > Mod::get()->getSettingValue<int64_t>("cache-limit")) {
            ProxyHandler* back = ProxyHandler::ALIVE_PROXIES.back();

            if (back->m_finished) {
                delete back;
            }

            ProxyHandler::ALIVE_PROXIES.pop_back();
        }
    }

    if (proxy->m_info->isPaused()) {
        std::lock_guard pauseLock(ProxyHandler::PAUSED_MUTEX);

        ProxyHandler::PAUSED_PROXIES.emplace_back(proxy);
    }

    ProxyEvent(EventState::REQUEST).send(proxy->m_info);
}

ProxyHandler::ProxyHandler(CCHttpRequest* request) : m_cocosRequest(request),
m_info(std::make_shared<HttpInfo>(request)),
m_originalTarget(request->getTarget()),
m_originalProxy(request->getSelector()),
m_finished(false) {
    request->retain();
    request->setResponseCallback(this, httpresponse_selector(ProxyHandler::onCocosResponse));

    if (!m_info->isPaused()) {
        m_start = std::chrono::high_resolution_clock::now();

        CCHttpClient::getInstance()->send(request);
    }
}

ProxyHandler::ProxyHandler(std::string_view modID, web::WebRequest& request) : m_modRequest(request),
m_responseListener(web::IDBasedWebResponseEvent(request.getID()).listen([this](const web::WebResponse& response) {
    return this->onModResponse(response);
}, Priority::First)),
m_info(std::make_shared<HttpInfo>(modID == Mod::get()->getID(), request)),
m_originalTarget(nullptr),
m_originalProxy(nullptr),
m_finished(false),
m_start(std::chrono::high_resolution_clock::now()) { }

ProxyHandler::~ProxyHandler() {
    if (m_cocosRequest.has_value()) {
        m_cocosRequest.value()->release();
    }
}

std::shared_ptr<HttpInfo> ProxyHandler::getInfo() noexcept {
    return m_info;
}

size_t ProxyHandler::calculateResponseTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_start).count();
}

void ProxyHandler::onCocosResponse(CCHttpClient* client, CCHttpResponse* response) {
    m_info->m_response.emplace(HttpInfo::Response(&m_info->getRequest(), response, this->calculateResponseTime()));

    (m_originalTarget->*m_originalProxy)(client, response);
    this->onResponse();
}

bool ProxyHandler::onModResponse(const web::WebResponse& response) {
    m_info->m_response.emplace(HttpInfo::Response(&m_info->getRequest(), response, this->calculateResponseTime()));

    this->onResponse();

    return ListenerResult::Propagate;
}

void ProxyHandler::onResponse() {
    bool shouldRelease = false;
    m_finished = true;

    if (m_info->m_response->m_statusCode == static_cast<int>(web::GeodeWebError::REQUEST_CANCELLED)) {
        m_info->m_state = State::CANCELLED;
    } else if (m_info->m_response->m_statusCode < 0) {
        m_info->m_state = State::FAULTY;
    } else {
        m_info->m_state = State::COMPLETED;
    }

    ProxyEvent(EventState::RESPONSE).send(m_info);

    {
        std::shared_lock aliveLock(ProxyHandler::ALIVE_MUTEX);

        if (std::find_if(ProxyHandler::ALIVE_PROXIES.begin(), ProxyHandler::ALIVE_PROXIES.end(), [this](ProxyHandler* handler) {
            return handler->m_info->getID() == m_info->getID();
        }) == ProxyHandler::ALIVE_PROXIES.end()) {
            delete this;
        }
    }
}