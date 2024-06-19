#include "ThemeStyle.hpp"

const std::unordered_map<ThemeStyle::Theme, ThemeStyle> ThemeStyle::themes = {
    { DARK, {
        // Font
        "consola.fnt"_spr,
        // Font size
        0.4f,
        // Line height
        2,
        // Background
        { 0x1F, 0x1F, 0x1F },
        // Line
        { 0x6E, 0x76, 0x81 },
        // ScrollBorder
        { 0x01, 0x04, 0x09 },
        // Text
        { 0xCC, 0xCC, 0xCC },
        // Key
        { 0x9C, 0xDC, 0xFE },
        // String
        { 0xCE, 0x91, 0x78 },
        // Number
        { 0xB5, 0xCE, 0xA8 },
        // Constant
        { 0x56, 0x9C, 0xD6 },
        // Error
        { 0xF4, 0x47, 0x47 }
    } },
    { LIGHT, {
        // Font
        "consola.fnt"_spr,
        // Font size
        0.4f,
        // Line height
        2,
        // Background
        { 0xF8, 0xF8, 0xF8 },
        // Line
        { 0x6E, 0x76, 0x81 },
        // ScrollBorder
        { 0xE5, 0xE5, 0xE5 },
        // Text
        { 0x3B, 0x3B, 0x3B },
        // Key
        { 0x04, 0x51, 0xA5 },
        // String
        { 0xA3, 0x15, 0x15 },
        // Number
        { 0x09, 0x86, 0x58 },
        // Constant
        { 0x00, 0x00, 0xFF },
        // Error
        { 0xCD, 0x31, 0x31 }
    } }
};

const ThemeStyle& ThemeStyle::getTheme() {
    if (Mod::get()->getSettingValue<std::string>("theme") == "light") {
        return ThemeStyle::themes.at(LIGHT);
    } else {
        return ThemeStyle::themes.at(DARK);
    }
}