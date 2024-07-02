#include <Geode/utils/web.hpp>
#include "../include.hpp"

web::WebTask WebRequest_send(web::WebRequest* request, std::string_view method, std::string_view url) {
    log::info("{} {}", method, url);

    return request->send(method, url);
}

$execute {
    (void) Mod::get()->hook(
        reinterpret_cast<void*>(addresser::getNonVirtual(&web::WebRequest::send)),
        &WebRequest_send,
        "geode::web::WebRequest::send",
        tulip::hook::TulipConvention::Thiscall
    );
}