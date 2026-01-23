#include "../../../proxy/converters/RobTopToJsonV2.hpp"

const std::string proxy::converters::RobTopToJsonV2::Object::METADATA_KEY = "#";

const std::unordered_map<proxy::converters::RobTopToJsonV2::ObjectType, proxy::converters::RobTopToJsonV2::Object> proxy::converters::RobTopToJsonV2::PARSERS({
    { ObjectType::AS_IS, Object("\0") },
    { ObjectType::LEVEL_RESPONSE, Object(Object::METADATA_KEY, {
        { "level", ObjectType::LEVEL },
        { "hash1", ObjectType::AS_IS },
        { "hash2", ObjectType::AS_IS },
        { "dailyCreator", ObjectType::CREATOR },
        { "songs", ObjectType::SONGS }
    }) },
    { ObjectType::LEVEL, Object(":") },
    { ObjectType::CREATOR, Object(":", { "userID", "username", "accountID" }) },
    { ObjectType::SONGS, Object("~:~", ObjectType::SONG) },
    { ObjectType::SONG, Object("~|~") },
});

const std::unordered_map<std::string, proxy::converters::RobTopToJsonV2::ObjectType> proxy::converters::RobTopToJsonV2::ENDPOINT_MAPPINGS({
    { "/downloadGJLevel22.php", ObjectType::LEVEL_RESPONSE }
});

proxy::converters::RobTopToJsonV2::Object::Object(std::string_view delimiter) { }
proxy::converters::RobTopToJsonV2::Object::Object(std::string_view entryDelimiter, const ObjectType entryType) { }
proxy::converters::RobTopToJsonV2::Object::Object(std::string_view delimiter, const std::vector<std::string>& tuple) { }
proxy::converters::RobTopToJsonV2::Object::Object(std::string_view entryDelimiter, const std::vector<std::pair<std::string, ObjectType>>& tuple) { }

proxy::converters::RobTopToJsonV2::RobTopToJsonV2() : Converter(enums::ContentType::JSON) { };

bool proxy::converters::RobTopToJsonV2::canConvert(const std::string_view path, const bool isBody, const std::string_view original) const {
    return !isBody && RobTopToJsonV2::ENDPOINT_MAPPINGS.contains(std::string(path));
}

std::string proxy::converters::RobTopToJsonV2::convert(const std::string_view path, const std::string_view original) const { return ""; }
std::string proxy::converters::RobTopToJsonV2::toRaw(const std::string_view path, const std::string_view original) const { return ""; }