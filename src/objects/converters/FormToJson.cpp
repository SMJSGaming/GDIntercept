#include "FormToJson.hpp"

bool FormToJson::canConvert(const std::string& path, const std::string& original) {
    return original.find('=') != std::string::npos;
}

json FormToJson::convert(const std::string& path, const std::string& original) {
    json object(json::object());
    std::stringstream stream(original);
    std::string section;

    while (std::getline(stream, section, '&')) {
        const size_t equalPos = section.find('=');

        if (equalPos == std::string::npos) {
            object[section] = json();
        } else {
            const std::string key(section.substr(0, equalPos));

            object[key] = JsonConverter::getPrimitiveType(key, section.substr(equalPos + 1));
        }
    }

    return object;
}