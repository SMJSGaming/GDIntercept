#include "../../../proxy/converters/Converter.hpp"

using namespace nlohmann;
using namespace geode::prelude;

bool proxy::converters::isInt(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    const bool negative = str.at(0) == '-';

    return str.substr(negative, str.size() - negative).find_first_not_of("0123456789") == std::string::npos && (!negative || str.size() > 1);
}

bool proxy::converters::isNumber(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    const size_t decimalPos = str.find('.');

    return converters::isInt(str.substr(0, decimalPos)) && (
        decimalPos == std::string::npos ||
        str.substr(decimalPos + 1).find_first_not_of("0123456789") == std::string::npos
    );
}

bool proxy::converters::isBool(const std::string& str) {
    return str == "true" || str == "false";
}

bool proxy::converters::isNull(const std::string& str) {
    return str == "null";
}

bool proxy::converters::isString(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    const char firstChar = str.at(0);

    if ((firstChar == '\'' || firstChar == '"') && str.find('\n') == std::string::npos && str.ends_with(firstChar)) {
        std::stringstream stream(str.substr(1));
        std::string section;

        for (size_t i = 0; std::getline(stream, section, firstChar); i += section.size() + 1) {
            const bool eol = section.size() + 1 + i == str.size();
            const size_t lastNonEscape = section.find_last_not_of('\\') ;

            if ((lastNonEscape == std::string::npos ?  lastNonEscape : section.size()) % 2 == eol) {
                return false;
            }
        }

        return true;
    } else {
        return false;
    }
}

bool proxy::converters::shouldSanitize(const std::string& key) {
    static const std::vector<std::string> sensitiveKeys = {
        "password",
        "pass",
        "passwd",
        "pwd",
        "token",
        "tkn",
        "sID",
        "gjp",
        "gjp2"
    };

    return Mod::get()->getSettingValue<bool>("censor-data") && std::find(sensitiveKeys.begin(), sensitiveKeys.end(), key) != sensitiveKeys.end();
}

std::string proxy::converters::safeDump(const nlohmann::json& json, const size_t indent, const bool quoteless) {
    if (quoteless && json.is_string()) {
        return json.get<std::string>();
    } else {
        return json.dump(indent, ' ', true, json::error_handler_t::replace);
    }
}

nlohmann::json proxy::converters::getPrimitiveJsonType(const std::string& key, const std::string& str) {
    if (converters::shouldSanitize(key)) {
        return json("********");
    } else if (converters::isNull(str)) {
        return json();
    } else if (converters::isBool(str)) {
        return json(str == "true");
    } else if (converters::isInt(str)) {
        return json(std::stoll(str));
    } else if (converters::isNumber(str)) {
        return json(std::stold(str));
    } else if (converters::isString(str)) {
        return json(str.substr(1, str.size() - 2));
    } else {
        return json(str);
    }
}