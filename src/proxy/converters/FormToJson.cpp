#include "../../../proxy/converters/FormToJson.hpp"

using namespace nlohmann;

proxy::enums::ContentType proxy::converters::FormToJson::resultContentType() const {
    return enums::ContentType::JSON;
}

bool proxy::converters::FormToJson::needsSanitization() const {
    return true;
}

bool proxy::converters::FormToJson::canConvert(const std::string& path, const bool isBody, const std::string& original) const {
    std::stringstream stream(original);
    std::string section;

    while (std::getline(stream, section, '&')) {
        // The official form spec has no null value support and an empty string still requires an empty assignment, thus this is invalid
        if (section.find('=') == std::string::npos) {
            return false;
        }
    }

    return true;
}

std::string proxy::converters::FormToJson::convert(const std::string& path, const std::string& original) const {
    json object(json::object());
    std::stringstream stream(original);
    std::string section;

    while (std::getline(stream, section, '&')) {
        const size_t equalPos = section.find('=');
        const std::string key = section.substr(0, equalPos);

        if (equalPos == std::string::npos) {
            object[key] = json("");
        } else {
            object[key] = getPrimitiveJsonType(key, section.substr(equalPos + 1));
        }
    }

    return converters::safeDump(object);
}

std::string proxy::converters::FormToJson::toRaw(const std::string& path, const std::string& original) const {
    const json object = json::parse(original);
    std::stringstream result;

    for (const auto& [key, value] : object.items()) {
        if (result.tellp() > 0) {
            result << '&';
        }

        result << key << '=' << converters::safeDump(value, 0, true);
    }

    return result.str();
}