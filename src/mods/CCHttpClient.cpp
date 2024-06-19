#include "CCHttpClient.hpp"

void ModCCHttpClient::send(CCHttpRequest* request) {
    HttpInfo* requestInfo = HttpInfo::create(request);

    log::info("Sending request:\n{}\nQuery: {}\nHeaders: {}\nBody: {}",
        requestInfo->generateBasicInfo(),
        requestInfo->formatQuery(),
        requestInfo->formatHeaders(),
        requestInfo->formatBody().second
    );

    CCHttpClient::send(request);
}