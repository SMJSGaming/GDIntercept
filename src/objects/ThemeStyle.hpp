#pragma once

#include "../include.hpp"

struct ThemeStyle {
    enum Theme {
        DARK,
        LIGHT
    };

    static const ThemeStyle& getTheme();

    const char* fontName;
    float fontSize;
    float lineHeight;
    cocos2d::ccColor3B background;
    cocos2d::ccColor3B line;
    cocos2d::ccColor3B scrollBorder;
    cocos2d::ccColor3B text;
    cocos2d::ccColor3B key;
    cocos2d::ccColor3B string;
    cocos2d::ccColor3B number;
    cocos2d::ccColor3B constant;
    cocos2d::ccColor3B error;
private:
    static const std::unordered_map<Theme, ThemeStyle> themes;
};