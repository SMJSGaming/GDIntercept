#include "CCHttpClient.hpp"

using namespace proxy::prelude;

void ModCCHttpClient::send(CCHttpRequest* request) {
    if (ProxyHandler::isProxy(request)) {
        CCHttpClient::send(request);
    } else {
        ProxyHandler::create(request);
    }
}