#include "CCHttpClient.hpp"

void ModCCHttpClient::send(CCHttpRequest* request) {
    HttpInfo* requestInfo = HttpInfo::create(request);

    if (Mod::get()->getSettingValue<bool>("log-requests")) {
        log::info("Sending request:\n{}\nQuery: {}\nHeaders: {}\nBody: {}",
            requestInfo->generateBasicInfo(false),
            requestInfo->formatQuery(),
            requestInfo->formatHeaders(),
            requestInfo->formatBody().second
        );
    }

    CCHttpClient::send(request);
}