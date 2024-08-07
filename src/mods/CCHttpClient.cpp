#include "CCHttpClient.hpp"

void ModCCHttpClient::send(CCHttpRequest* request) {
    bool isProxy = false;

    for (ProxyHandler* proxy : ProxyHandler::getAliveProxies()) {
        if (proxy->getCocosRequest() == request) {
            isProxy = true;

            break;
        }
    }

    if (isProxy) {
        CCHttpClient::send(request);
    } else {
        ProxyHandler::create(request);
    }

}