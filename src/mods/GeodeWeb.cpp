#include <Geode/utils/web.hpp>
#include "../include.hpp"

web::WebTask WebRequest_send(web::WebRequest* request, std::string_view method, std::string_view url) {
    if (ProxyHandler::isProxy(request)) {
        return request->send(method, url);
    } else {
        return ProxyHandler::create(request, std::string(method), std::string(url))->getModTask();
    }
}

$execute {
    (void) Mod::get()->hook(
        reinterpret_cast<void*>(addresser::getNonVirtual(&web::WebRequest::send)),
        &WebRequest_send,
        "geode::web::WebRequest::send",
        tulip::hook::TulipConvention::Thiscall
    );
}