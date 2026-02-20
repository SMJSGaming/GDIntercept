#pragma once

#include "../include.hpp"
#include "../proxy/Proxy.hpp"

class Warning {
public:
    static void show();
    static void hide();
private:
    static CCScale9Sprite* WARNING;
};