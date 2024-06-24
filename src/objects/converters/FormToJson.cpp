#include "FormToJson.hpp"

bool FormToJson::canConvert(const std::string& path, const std::string& original) {
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

json FormToJson::convert(const std::string& path, const std::string& original) {
    json object(json::object());
    std::stringstream stream(original);
    std::string section;

    while (std::getline(stream, section, '&')) {
        const size_t equalPos = section.find('=');

        if (equalPos == std::string::npos) {
            return json();
        } else {
            const std::string key(section.substr(0, equalPos));

            object[key] = JsonConverter::getPrimitiveType(key, section.substr(equalPos + 1));
        }
    }

    return object;
}