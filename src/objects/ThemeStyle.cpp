#include "ThemeStyle.hpp"

const std::unordered_map<std::string, ThemeStyle> ThemeStyle::themes = {
    { "Dark", {
        .font = {
            .fontName = "consola.fnt"_spr,
            .fontScale = 0.4f,
            .lineHeight = 2
        },
        .syntax = {
            .text = 0xCCCCCC,
            .key = 0x9CDCFE,
            .string = 0xCE9178,
            .number = 0xB5CEA8,
            .constant = 0x569CD6,
            .error = 0xF44747,
            .separator = 0xCCCCCC,
            .terminator = 0xCCCCCC,
            .bracket = 0xCCCCCC
        },
        .ui = {
            .background = 0x1F1F1F,
            .foreground = 0x6E7681,
            .menu = {
                .background = 0x181818,
                .foreground = 0x37373D,
                .border = 0x2B2B2B
            },
            .scrollbar = {
                .thumb = 0x6E7681,
                .border = 0x000000
            }
        }
    } },
    { "Light", {
        .font = {
            .fontName = "consola.fnt"_spr,
            .fontScale = 0.4f,
            .lineHeight = 2
        },
        .syntax = {
            .text = 0x3B3B3B,
            .key = 0x0451A5,
            .string = 0xA31515,
            .number = 0x098658,
            .constant = 0x0000FF,
            .error = 0xCD3131,
            .separator = 0x3B3B3B,
            .terminator = 0x3B3B3B,
            .bracket = 0x3B3B3B
        },
        .ui = {
            .background = 0xFFFFFF,
            .foreground = 0x6E7681,
            .menu = {
                .background = 0xF8F8F8,
                .foreground = 0x616161,
                .border = 0xE5E5E5
            },
            .scrollbar = {
                .thumb = 0x6E7681,
                .border = 0xE5E5E5
            }
        }
    } },
    { "Dracula", {
        .font = {
            .fontName = "consola.fnt"_spr,
            .fontScale = 0.4f,
            .lineHeight = 2
        },
        .syntax = {
            .text = 0xF8F8F2,
            .key = 0x8BE9FD,
            .string = 0xF1FA8C,
            .number = 0xBD93F9,
            .constant = 0xBD93F9,
            .error = 0xFF5555,
            .separator = 0xFF79C6,
            .terminator = 0xF8F8F2,
            .bracket = 0xFF79C6
        },
        .ui = {
            .background = 0x282A36,
            .foreground = 0xF8F8F2,
            .menu = {
                .background = 0x21222C,
                .foreground = 0x44475A,
                .border = Color::INVISIBLE
            },
            .scrollbar = {
                .thumb = 0xF8F8F2,
                .border = 0x191A21
            }
        }
    } }
};

const ThemeStyle& ThemeStyle::getTheme() {
    const std::string theme = Mod::get()->getSettingValue<std::string>("theme");

    if (!ThemeStyle::themes.contains(theme)) {
        return ThemeStyle::themes.at("Dark");
    }

    return ThemeStyle::themes.at(theme);
}

const uint32_t ThemeStyle::Color::COLOR3B_MAX = 0xFFFFFF;

const ThemeStyle::Color ThemeStyle::Color::INVISIBLE = { 0, 0, 0, 0 };

ThemeStyle::Color::operator float() const {
    return a;
}

ThemeStyle::Color::operator ccColor3B() const {
    return { r, g, b };
}

ThemeStyle::Color::operator ccColor4B() const {
    return { r, g, b, a };
}

ThemeStyle::Color::Color(const uint32_t hex) : r((hex >> 16) & 0xFF),
g((hex >> 8) & 0xFF),
b(hex & 0xFF),
a(hex > Color::COLOR3B_MAX ? (hex >> 24) & 0xFF : FULL_OPACITY) { }

ThemeStyle::Color::Color(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a) : r(r), g(g), b(b), a(a) { }