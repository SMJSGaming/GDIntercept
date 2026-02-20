#pragma once

#include <Geode/modify/MenuLayer.hpp>
#include "../include.hpp"
#include "../proxy/Proxy.hpp"
#include "../nodes/Warning.hpp"
#include "../nodes/InterceptPopup.hpp"

class $modify(ModMenuLayer, MenuLayer) {
    bool init();
    void onLogoClick(CCObject* sender);
};