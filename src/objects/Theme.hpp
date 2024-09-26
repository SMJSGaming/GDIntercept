#pragma once

#include <Geode/Geode.hpp>
#include "../macro_utils.hpp"
#include "../../lib/json.hpp"
#include "../../lib/LookupTable.hpp"
#include "../settings/DynamicEnum.hpp"

using namespace nlohmann;
using namespace geode::prelude;

template<typename T>
concept ColorNode = requires(T color) {
    { color.setOpacity(float()) };
    { color.setColor(ccColor3B()) };
};

namespace Theme {
    struct Color {
        operator float() const;
        operator ccColor3B() const;
        operator ccColor4B() const;

        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;

        Color(const GLubyte a = 0);
        Color(const std::string hex);
        Color(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a = 255);

        bool isInvisible() const;
        bool isTransparent() const;
        bool isOpaque() const;
        template<ColorNode T>
        void applyTo(T* node) const {
            node->setColor(*this);
            node->setOpacity(*this);
        }
    private:
        // Supporting the format #R/G/B, #RR/GG/BB, #RGB, #RGBA, #RRGGBB and #RRGGBBAA
        void extractChannels(const std::string& hex);
        GLubyte extractChannel(const std::string& hexString, const size_t index, const bool isShort) const;

        friend void from_json(const json& json, Color& color);
    };

    struct Font {
        std::string fontName;
        float fontScale;
        float lineHeight;

        CCLabelBMFont* createLabel(const std::string& text) const;
        CCSize getTrueFontSize() const;
    };

    struct Code {
        struct Scrollbar {
            float size;
            Color border;
            Color thumb;
            Color activeThumb;
        };

        struct Syntax {
            Color text;
            Color key;
            Color string;
            Color number;
            Color constant;
            Color error;
            Color separator;
            Color terminator;
            Color bracket;
            Color keyQuote;
            Color stringQuote;
        };

        float paddingLeft;
        float paddingRight;
        float paddingCenter;
        Color background;
        Color foreground;
        Font font;
        Scrollbar scrollbar;
        Syntax syntax;
    };

    struct Menu {
        struct Icons {
            Color info;
            Color body;
            Color query;
            Color header;
            Color response;
            Color actionIcon;
            Color disabledIcon;
            Color categoryIcon;
        };

        float paddingLeft;
        float paddingRight;
        float paddingCenter;
        Color background;
        Color categoryBackground;
        Color foreground;
        Color text;
        Color disabledText;
        Color categoryText;
        Color border;
        Color categoryBorder;
        Font font;
        Icons icons;
    };

    struct Theme {
        static void load();

        Code code;
        Menu menu;
    private:
        static LookupTable<std::string, Theme> cachedThemes;

        static void loadDirectory(const std::filesystem::path& path);
        static Theme createTheme(const json& obj);
        static Color colorOrDefault(const json& obj, const std::string& key, const Color& override, const std::optional<GLubyte>& alpha = std::nullopt);

        template <typename T>
        static T objectOrDefault(const json& obj, const std::string& key, const std::function<T(const json& obj)>& constructor, const T& defaultObj) {
            return obj.contains(key) ? constructor(obj.at(key)) : defaultObj;
        }

        friend const Theme getTheme();
    };

    const Theme getTheme();
    void from_json(const json& json, Color& color);
};