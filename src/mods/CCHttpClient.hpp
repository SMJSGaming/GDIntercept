#pragma once

#include <Geode/modify/CCHttpClient.hpp>
#include "../include.hpp"
#include "../proxy/ProxyHandler.hpp"

class $modify(ModCCHttpClient, CCHttpClient) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::extension::CCHttpClient::send", 999999999);
    }

    void send(CCHttpRequest* request);
};