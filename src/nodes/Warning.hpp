#pragma once

#include "../include.hpp"

class Warning {
public:
    static void show();
    static void hide();
private:
    static CCScale9Sprite* warning;
};