#include "CCHttpClient.hpp"

void ModCCHttpClient::send(CCHttpRequest* request) {
    if (ProxyHandler::isProxy(request)) {
        CCHttpClient::send(request);
    } else {
        ProxyHandler::create(request);
    }
}