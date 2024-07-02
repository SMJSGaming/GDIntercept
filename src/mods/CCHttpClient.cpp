#include "CCHttpClient.hpp"

void ModCCHttpClient::send(CCHttpRequest* request) {
    ProxyHandler* proxy = ProxyHandler::create(request);

    context::registerRequest(proxy);
    CCHttpClient::send(request);
}