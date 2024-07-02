#include "CCHttpClient.hpp"

void ModCCHttpClient::send(CCHttpRequest* request) {
    bool isProxy = false;

    for (ProxyHandler* proxy : ProxyHandler::getProxies()) {
        if (proxy->getCocosRequest() == request) {
            isProxy = true;

            break;
        }
    }

    if (!isProxy) {
        ProxyHandler::create(request);
    }

    if (!Mod::get()->getSettingValue<bool>("pause-requests")) {
        CCHttpClient::send(request);
    }
}