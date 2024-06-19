#pragma once

#include <Geode/modify/CCHttpClient.hpp>
#include "../include.hpp"
#include "../objects/HttpInfo.hpp"

class $modify(ModCCHttpClient, CCHttpClient) {
    void send(CCHttpRequest* request);
};