#include "../../../proxy/converters/JsonToJson.hpp"

using namespace nlohmann;

proxy::converters::JsonToJson::JsonToJson() : Converter(enums::ContentType::JSON) { };

bool proxy::converters::JsonToJson::canConvert(const std::string_view path, const bool isBody, const std::string_view original) const {
    return json::accept(original);
}

std::string proxy::converters::JsonToJson::convert(const std::string_view path, const std::string_view original) const {
    json object = json::parse(original);

    recursiveSanitize(object);

    return converters::safeDump(object);
}

std::string proxy::converters::JsonToJson::toRaw(const std::string_view path, const std::string_view original) const {
    return converters::safeDump(json::parse(original), -1);
}

void proxy::converters::JsonToJson::recursiveSanitize(json& object) const {
    for (const auto& [key, value] : object.items()) {
        if (value.is_array() || value.is_object()) {
            recursiveSanitize(value);
        } else if (converters::shouldSanitize(key)) {
            object[key] = json("********");
        }
    }
}