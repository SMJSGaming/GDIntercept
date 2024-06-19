#include "JsonConverter.hpp"

bool JsonConverter::isInt(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    const bool negative = str.at(0) == '-';

    return str.substr(negative, str.size() - negative).find_first_not_of("0123456789") == std::string::npos && (!negative || str.size() > 1);
}

bool JsonConverter::isNumber(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    const size_t decimalPos = str.find('.');

    return JsonConverter::isInt(str.substr(0, decimalPos)) && (
        decimalPos == std::string::npos ||
        str.substr(decimalPos + 1).find_first_not_of("0123456789") == std::string::npos
    );
}

bool JsonConverter::isBool(const std::string& str) {
    return str == "true" || str == "false";
}

bool JsonConverter::isNull(const std::string& str) {
    return str == "null";
}

bool JsonConverter::isString(const std::string& str) {
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

bool JsonConverter::isJson(const std::string& str) {
    return str.starts_with('{') ||
        str.starts_with('[') ||
        JsonConverter::isNumber(str) ||
        JsonConverter::isString(str) ||
        JsonConverter::isBool(str) ||
        JsonConverter::isNull(str);
}

json JsonConverter::getPrimitiveType(const std::string& key, const std::string& str) {
    static const std::vector<std::string> sensitiveKeys({ "password", "pass", "passwd", "pwd", "token", "tkn", "sID", "gjp", "gjp2" });

    if (Mod::get()->getSettingValue<bool>("censor-data") && std::find(sensitiveKeys.begin(), sensitiveKeys.end(), key) != sensitiveKeys.end()) {
        return json("********");
    } else if (JsonConverter::isNull(str)) {
        return json();
    } else if (JsonConverter::isBool(str)) {
        return json(str == "true");
    } else if (JsonConverter::isInt(str)) {
        return json(std::stoll(str));
    } else if (JsonConverter::isNumber(str)) {
        return json(std::stold(str));
    } else if (JsonConverter::isString(str)) {
        return json(str.substr(1, str.size() - 2));
    } else {
        return json(str);
    }
}