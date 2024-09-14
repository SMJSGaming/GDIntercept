#pragma once

#include "../include.hpp"

struct ThemeStyle {
    struct Color {
        static const uint32_t COLOR3B_MAX;
        static const Color INVISIBLE;

        operator float() const;
        operator ccColor3B() const;
        operator ccColor4B() const;

        const GLubyte r;
        const GLubyte g;
        const GLubyte b;
        const GLubyte a;

        // 0xAARRGGBB
        Color(const uint32_t hex);
        Color(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a = FULL_OPACITY);
    };

    struct Font {
        const char* fontName;
        const float fontScale;
        const float lineHeight;
    };

    struct Syntax {
        const Color text;
        const Color key;
        const Color string;
        const Color number;
        const Color constant;
        const Color error;
        const Color separator;
        const Color terminator;
        const Color bracket;
    };

    struct UI {
        struct Menu {
            // activityBar.background
            const Color background;
            // list.inactiveForeground
            const Color foreground;
            // activityBar.border
            const Color border;
        };

        struct Scrollbar {
            // scrollbarSlider.background
            const Color thumb;
            // editorOverviewRuler.border
            const Color border;
        };

        // editor.background
        const Color background;
        // editor.foreground
        const Color foreground;
        const Menu menu;
        const Scrollbar scrollbar;
    };

    static const ThemeStyle& getTheme();

    const Font font;
    const Syntax syntax;
    const UI ui;
private:
    static const std::unordered_map<std::string, ThemeStyle> themes;
};