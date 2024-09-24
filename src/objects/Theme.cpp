#include "Theme.hpp"

LookupTable<std::string, Theme::Theme> Theme::Theme::cachedThemes;

Theme::Color::operator float() const {
    return a;
}

Theme::Color::operator ccColor3B() const {
    return { r, g, b };
}

Theme::Color::operator ccColor4B() const {
    return { r, g, b, a };
}

Theme::Color::Color(const GLubyte a) : r(0), g(0), b(0), a(a) { }

Theme::Color::Color(const std::string hex) : r(0), g(0), b(0), a(255) {
    this->extractChannels(hex);
}

Theme::Color::Color(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a) : r(r), g(g), b(b), a(a) { }

bool Theme::Color::isInvisible() const {
    return a == 0;
}

bool Theme::Color::isTransparent() const {
    return a > 0 && a < 255;
}

bool Theme::Color::isOpaque() const {
    return a == 255;
}

void Theme::Color::extractChannels(const std::string& hex) {
    ESCAPE_WHEN(hex.size() < 2,);

    const std::string hexStr = hex.substr(1);
    const size_t size = hexStr.size();

    ESCAPE_WHEN(!hex.starts_with("#") || hexStr.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos || (size > 4 && size != 6 && size != 8),);

    if (size < 3) {
        r = g = b = extractChannel(hexStr, 0, size == 1);
        a = 255;
    } else {
        const bool isShort = size < 6;

        r = extractChannel(hexStr, 0, isShort);
        g = extractChannel(hexStr, 1, isShort);
        b = extractChannel(hexStr, 2, isShort);

        if (size % 4 == 0) {
            a = extractChannel(hexStr, 3, isShort);
        } else {
            a = 255;
        }
    }
}

GLubyte Theme::Color::extractChannel(const std::string& hexString, const size_t index, const bool isShort) const {
    const size_t channelSize = 1 + !isShort;
    const std::string channel = hexString.substr(channelSize * index, channelSize);

    if (isShort) {
        return std::stoi(channel + channel, nullptr, 16);
    } else {
        return std::stoi(channel, nullptr, 16);
    }
}

CCLabelBMFont* Theme::Font::createLabel(const std::string& text) const {
    CCLabelBMFont* label = CCLabelBMFont::create(text.c_str(), fontName.c_str());

    label->setScale(fontScale);

    return label;
}

CCSize Theme::Font::getTrueFontSize() const {
    return this->createLabel("0")->getScaledContentSize();
}

void Theme::Theme::load() {
    Mod* mod = Mod::get();
    const std::filesystem::path themesDir = mod->getConfigDir() / "themes";

    (void) utils::file::createDirectoryAll(themesDir);

    Theme::cachedThemes.clear();
    Theme::loadDirectory(mod->getResourcesDir());
    Theme::loadDirectory(themesDir);
    mod->setSavedValue("themes", Theme::cachedThemes.keys());
}

void Theme::Theme::loadDirectory(const std::filesystem::path& path) {
    const Result<std::vector<std::filesystem::path>> dir = utils::file::readDirectory(path);

    ESCAPE_WHEN(dir.isErr(), log::warn("Failed to read directory {}: {}", path.string(), dir.err().value_or("Unknown error")));

    for (const auto& file : dir.unwrap()) {
        CONTINUE_WHEN(file.extension() != ".json");

        const Result<std::string> text = utils::file::readString(file);

        if (text.isErr()) {
            log::warn("Failed to read theme file {}: {}", file.string(), text.err().value_or("Unknown error"));
            continue;
        }

        const json theme = json::parse(text.unwrap());

        if (theme.contains("name") && theme.contains("theme")) try {
            Theme::cachedThemes.insert(theme.at("name").get<std::string>(), Theme::createTheme(theme.at("theme")));
        } catch (const json::exception& e) {
            log::warn("Failed to parse theme file {}: {}", file.string(), e.what());
        } else {
            log::info("Skipped parsing JSON file {}", file.string());
        }
    }
}

Theme::Theme Theme::Theme::createTheme(const json& obj) {
    const json code = obj.at("code");
    const json menu = obj.at("menu");
    const json codeFont = code.at("font");
    const json scrollbar = code.at("scrollbar");
    const json syntax = code.at("syntax");
    const json menuFont = menu.at("font");
    const Color thumb = scrollbar.at("thumb");
    const Color syntaxText = syntax.at("text");
    const Color syntaxKey = syntax.at("key");
    const Color syntaxString = syntax.at("string");
    const Color menuBackground = menu.at("background");
    const Color menuText = menu.at("text");
    const Color menuBorder = menu.at("border");
    const Color disabledText = Theme::colorOrDefault(menu, "disabled-text", menuText, 0x55);
    const Color categoryText = Theme::colorOrDefault(menu, "category-text", menuText, 0xAA);
    const Menu::Icons defaultIcons = Menu::Icons({
        .info = Color("#FF6961"),
        .body = Color("#FFEE8C"),
        .query = Color("#A2BFFE"),
        .header = Color("#B19CD9"),
        .response = Color("#B2FBA5"),
        .actionIcon = menuText,
        .disabledIcon = disabledText,
        .categoryIcon = categoryText
    });

    return {
        .code = {
            .paddingLeft = Theme::objectOrDefault<float>(code, "padding-left", [](const json& obj) { return obj.get<float>(); }, 3),
            .paddingRight = Theme::objectOrDefault<float>(code, "padding-right", [](const json& obj) { return obj.get<float>(); }, 3),
            .paddingCenter = Theme::objectOrDefault<float>(code, "padding-center", [](const json& obj) { return obj.get<float>(); }, 3),
            .background = code.at("background"),
            .foreground = code.at("foreground"),
            .font = {
                .fontName = codeFont.at("family").get<std::string>(),
                .fontScale = codeFont.at("scale").get<float>(),
                .lineHeight = Theme::objectOrDefault<float>(codeFont, "line-height", [](const json& obj) { return obj.get<float>(); }, 2)
            },
            .scrollbar = {
                .size = Theme::objectOrDefault<float>(scrollbar, "size", [](const json& obj) { return obj.get<float>(); }, 4),
                .border = scrollbar.at("border"),
                .thumb = thumb,
                .activeThumb = Theme::colorOrDefault(scrollbar, "active-thumb", thumb, std::min<GLubyte>(thumb * 2, 255))
            },
            .syntax = {
                .text = syntaxText,
                .key = syntaxKey,
                .string = syntaxString,
                .number = syntax.at("number"),
                .constant = syntax.at("constant"),
                .error = syntax.at("error"),
                .separator = Theme::colorOrDefault(syntax, "separator", syntaxText),
                .terminator = Theme::colorOrDefault(syntax, "terminator", syntaxText),
                .bracket = Theme::colorOrDefault(syntax, "bracket", syntaxText),
                .keyQuote = Theme::colorOrDefault(syntax, "key-quote", syntaxKey),
                .stringQuote = Theme::colorOrDefault(syntax, "string-quote", syntaxString)
            }
        },
        .menu = {
            .paddingLeft = Theme::objectOrDefault<float>(menu, "padding-left", [](const json& obj) { return obj.get<float>(); }, 3),
            .paddingRight = Theme::objectOrDefault<float>(menu, "padding-right", [](const json& obj) { return obj.get<float>(); }, 3),
            .paddingCenter = Theme::objectOrDefault<float>(menu, "padding-center", [](const json& obj) { return obj.get<float>(); }, 3),
            .background = menu.at("background"),
            .categoryBackground = Theme::colorOrDefault(menu, "category-background", menuBackground),
            .foreground = menu.at("foreground"),
            .text = menuText,
            .disabledText = disabledText,
            .categoryText = categoryText,
            .border = menuBorder,
            .categoryBorder = Theme::colorOrDefault(menu, "category-border", menuBorder),
            .font = {
                .fontName = menuFont.at("family").get<std::string>(),
                .fontScale = menuFont.at("scale").get<float>(),
                .lineHeight = Theme::objectOrDefault<float>(menuFont, "line-height", [](const json& obj) { return obj.get<float>(); }, 2)
            },
            .icons = Theme::objectOrDefault<Menu::Icons>(menu, "icons", [defaultIcons](const json& obj) {
                return Menu::Icons({
                    .info = Theme::colorOrDefault(obj, "info", defaultIcons.info),
                    .body = Theme::colorOrDefault(obj, "body", defaultIcons.body),
                    .query = Theme::colorOrDefault(obj, "query", defaultIcons.query),
                    .header = Theme::colorOrDefault(obj, "header", defaultIcons.header),
                    .response = Theme::colorOrDefault(obj, "response", defaultIcons.response),
                    .actionIcon = Theme::colorOrDefault(obj, "action-icon", defaultIcons.actionIcon),
                    .disabledIcon = Theme::colorOrDefault(obj, "disabled-icon", defaultIcons.disabledIcon),
                    .categoryIcon = Theme::colorOrDefault(obj, "category-icon", defaultIcons.categoryIcon)
                });
            }, defaultIcons)
        }
    };
}

Theme::Color Theme::Theme::colorOrDefault(const json& obj, const std::string& key, const Color &override, const std::optional<GLubyte>& alpha) {
    if (obj.contains(key)) {
        return obj.at(key);
    } else {
        Color colorCopy = Color(override);

        if (alpha.has_value()) {
            colorCopy.a = alpha.value();
        }

        return colorCopy;
    }
}

const Theme::Theme Theme::getTheme() {
    const std::string theme = Mod::get()->getSettingValue<DynamicEnumValue>("theme");

    if (Theme::cachedThemes.contains(theme)) {
        return Theme::cachedThemes.at(theme);
    } else {
        // Dark is a default theme and will always exist unless someone messes with the default themes, in which case I can't guarantee anything
        return Theme::cachedThemes.at("Dark");
    }
}

void Theme::from_json(const json& json, Color& color) {
    if (json.is_array()) {
        json.at(0).get_to(color.r);
        json.at(1).get_to(color.g);
        json.at(2).get_to(color.b);

        if (json.size() > 3) {
            json.at(3).get_to(color.a);
        }
    } else if (json.is_object()) {
        json.at("r").get_to(color.r);
        json.at("g").get_to(color.g);
        json.at("b").get_to(color.b);

        if (json.contains("a")) {
            json.at("a").get_to(color.a);
        }
    } else if (json.is_string()) {
        color.extractChannels(json.get<std::string>());
    } else {
        log::warn("Failed to parse color: {}", json.dump(2, ' ', true, json::error_handler_t::replace));
    }
}