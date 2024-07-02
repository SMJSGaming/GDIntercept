#include "include.hpp"

std::vector<ProxyHandler*> context::CACHED_PROXIES;

void context::registerRequest(ProxyHandler* proxy) {
    if (!Mod::get()->getSettingValue<bool>("remember-requests") && CACHED_PROXIES.size() > 0) {
        delete CACHED_PROXIES.at(0);

        CACHED_PROXIES.resize(0);
    }

    CACHED_PROXIES.insert(CACHED_PROXIES.begin(), proxy);
}