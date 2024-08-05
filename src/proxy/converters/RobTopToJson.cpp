#include "../../../proxy/converters/RobTopToJson.hpp"

using namespace nlohmann;

const std::unordered_map<std::string, proxy::converters::RobTopToJson::Parser> proxy::converters::RobTopToJson::parsers({
    // Accounts
    { "/database/accounts/syncGJAccountNew.php", RobTopToJson::Parser(std::vector<std::string>({ "encodedResponse" })) },
    { "/database/accounts/loginGJAccount.php", RobTopToJson::Parser({ "accountID", "userID" }, ",") },
    // Users
    { "/database/getGJScores20.php", RobTopToJson::Parser(":", "|") },
    { "/database/getGJUserInfo20.php", RobTopToJson::Parser(":") },
    { "/database/getGJUsers20.php", RobTopToJson::Parser(":", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    // Levels
    { "/database/downloadGJLevel22.php", RobTopToJson::Parser(":", {
        { "hash1", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) },
        { "hash2", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) },
        { "unknown", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) },
        { "songs", RobTopToJson::ObjParser("~|~", "~:~") }
    }) },
    { "/database/getGJDailyLevel.php", RobTopToJson::Parser({ "tempID", "secondsLeft" }, "|") },
    { "/database/getGJGauntlets21.php", RobTopToJson::Parser(":", "|", {
        { "hash", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) }
    }) },
    { "/database/getGJLevels21.php", RobTopToJson::Parser(":", "|", {
        { "creators", RobTopToJson::ObjParser({ "userID", "username", "accountID" }, ":", "|") },
        { "songs", RobTopToJson::ObjParser("~|~", "~:~") },
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") },
        { "hash", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) }
    }) },
    { "/database/getGJLevelLists.php", RobTopToJson::Parser(":", "|", {
        { "creators", RobTopToJson::ObjParser({ "userID", "username", "accountID" }, ":", "|") },
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") },
        { "hash", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) }
    }) },
    { "/database/getGJLevelScores211.php", RobTopToJson::Parser(":", "|") },
    { "/database/getGJLevelScoresPlat.php", RobTopToJson::Parser(":", "|") },
    { "/database/getGJMapPacks21.php", RobTopToJson::Parser(":", "|", {
        { "hash", RobTopToJson::ObjParser(std::vector<std::string>({ "string" })) }
    }) },
    // Comments
    { "/database/getGJAccountComments20.php", RobTopToJson::Parser("~", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    { "/database/getGJCommentHistory.php", RobTopToJson::Parser("~", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    { "/database/getGJComments21.php", RobTopToJson::Parser("~", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    // Socials
    { "/database/downloadGJMessage20.php", RobTopToJson::Parser(":") },
    { "/database/getGJFriendRequests20.php", RobTopToJson::Parser(":", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    { "/database/getGJMessages20.php", RobTopToJson::Parser(":", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    { "/database/getGJUserList20.php", RobTopToJson::Parser(":", "|") },
    // Rewards
    { "/database/getGJChallenges.php", RobTopToJson::Parser({ "encodedResponse", "hash" }, "|") },
    { "/database/getGJRewards.php", RobTopToJson::Parser({ "encodedResponse", "hash" }, "|") },
    // Songs
    { "/database/getGJSongInfo.php", RobTopToJson::Parser("~|~") },
    { "/database/getGJTopArtists.php", RobTopToJson::Parser(":", "|", {
        { "pagination", RobTopToJson::ObjParser({ "total", "offset", "amount" }, ":") }
    }) },
    // Misc
    { "/database/getSaveData.php", RobTopToJson::Parser(std::vector<std::string>({ "encodedResponse" })) }
});

proxy::converters::RobTopToJson::ObjParser::ObjParser(const char* delimiter, const char* entryDelimiter) :
m_delimiter(delimiter),
m_entryDelimiter(entryDelimiter) {
    m_bodyType = m_entryDelimiter.empty() ? ARRAY : OBJECT;
}

proxy::converters::RobTopToJson::ObjParser::ObjParser(const std::vector<std::string>& tupleKeys, const char* delimiter, const char* entryDelimiter) :
m_tupleKeys(tupleKeys),
m_delimiter(delimiter),
m_entryDelimiter(entryDelimiter) {
    m_bodyType = m_entryDelimiter.empty() ? ARRAY : OBJECT;
}

proxy::converters::RobTopToJson::Parser::Parser(const char* delimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys) :
ObjParser(delimiter),
m_metadata(metadataKeys) {}

proxy::converters::RobTopToJson::Parser::Parser(const std::vector<std::string>& tupleKeys, const char* delimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys) :
ObjParser(tupleKeys, delimiter),
m_metadata(metadataKeys) {}

proxy::converters::RobTopToJson::Parser::Parser(const char* delimiter, const char* entryDelimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys) :
ObjParser(delimiter, entryDelimiter),
m_metadata(metadataKeys) {}

proxy::converters::RobTopToJson::Parser::Parser(const std::vector<std::string>& tupleKeys, const char* delimiter, const char* entryDelimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys) :
ObjParser(tupleKeys, delimiter, entryDelimiter),
m_metadata(metadataKeys) {}

std::vector<std::string> proxy::converters::RobTopToJson::ObjParser::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> parts;

    if (delimiter.empty()) {
        parts.push_back(str);
    } else {
        size_t end, start = 0;

        while ((end = str.find(delimiter, start)) != std::string::npos) {
            parts.push_back(str.substr(start, end - start));

            start = end + delimiter.size();
        }

        parts.push_back(str.substr(start, end));
    }

    return parts;
}

json proxy::converters::RobTopToJson::ObjParser::parse(const std::string& str) const {
    if (m_entryDelimiter.empty()) {
        return parseEntry(str);
    } else {
        json array(json::array());

        for (const std::string& entry : this->split(str, m_entryDelimiter)) {
            // For some reason some responses have a trailing delimiter
            if (!entry.empty()) {
                array.push_back(parseEntry(entry));
            }
        }

        return array;
    }
}

json proxy::converters::RobTopToJson::ObjParser::parseEntry(const std::string& str) const {
    const std::vector<std::string> parts(this->split(str, m_delimiter));
    json object(json::object());
    std::string lastKey;

    for (size_t i = 0; i < parts.size(); i++) {
        if (m_tupleKeys.size()) {
            const std::string key(i < m_tupleKeys.size() ? m_tupleKeys.at(i) : fmt::format("unknown{}", i));

            object[key] = getPrimitiveJsonType(key, parts.at(i));
        } else if (i % 2 == 0) {
            object[lastKey = parts.at(i)] = json("");
        } else {
            object[lastKey] = getPrimitiveJsonType(lastKey, parts.at(i));
        }
    }

    return object;
}

std::string proxy::converters::RobTopToJson::ObjParser::toRaw(const json& json) const {
    std::stringstream stream;

    if (m_entryDelimiter.empty()) {
        stream << toRawEntry(json);
    } else {
        for (const auto& entry : json) {
            if (stream.tellp() != 0) {
                stream << m_entryDelimiter;
            }

            stream << toRawEntry(entry);
        }
    }

    return stream.str();
}

std::string proxy::converters::RobTopToJson::ObjParser::toRawEntry(const json& json) const {
    std::stringstream stream;

    if (m_tupleKeys.empty()) {
        for (const auto& [key, value] : json.items()) {
            if (stream.tellp() != 0) {
                stream << m_entryDelimiter;
            }

            stream << key << m_delimiter << converters::safeDump(value, -1, true);
        }
    } else {
        for (const std::string& key : m_tupleKeys) {
            if (stream.tellp() != 0) {
                stream << m_delimiter;
            }

            if (json.contains(key)) {
                stream << converters::safeDump(json.at(key), -1, true);
            }
        }
    }

    return stream.str();
}

json proxy::converters::RobTopToJson::Parser::parse(const std::string& str) const {
    if (m_metadata.empty()) {
        return ObjParser::parse(str);
    } else {
        std::stringstream stream(str);
        json object(json::object());
        std::string section;

        for (size_t i = 0; std::getline(stream, section, '#') && i <= m_metadata.size(); i++) {
            if (i == 0) {
                object["content"] = ObjParser::parse(section);
            } else {
                const auto& metadata = m_metadata.at(i - 1);

                object["metadata"][std::get<0>(metadata)] = std::get<1>(metadata).parse(section);
            }
        }

        return object;
    }
}

std::string proxy::converters::RobTopToJson::Parser::toRaw(const json& json) const {
    if (m_metadata.empty()) {
        return ObjParser::toRaw(json);
    } else {
        const auto& content = json.at("content");
        std::stringstream stream;

        stream << ObjParser::toRaw(content);

        for (const auto& metadata : m_metadata) {
            stream << "#" << std::get<1>(metadata).toRaw(json.at("metadata").at(std::get<0>(metadata)));
        }

        return stream.str();
    }
}

proxy::enums::ContentType proxy::converters::RobTopToJson::resultContentType() const {
    return enums::ContentType::JSON;
}

bool proxy::converters::RobTopToJson::needsSanitization() const {
    return true;
}

bool proxy::converters::RobTopToJson::canConvert(const std::string& path, const bool isBody, const std::string& original) const {
    return RobTopToJson::parsers.contains(path) && !isBody;
}

std::string proxy::converters::RobTopToJson::convert(const std::string& path, const std::string& original) const {
    return converters::safeDump(RobTopToJson::parsers.at(path).parse(original));
}

std::string proxy::converters::RobTopToJson::toRaw(const std::string& path, const std::string& original) const {
    return RobTopToJson::parsers.at(path).toRaw(json::parse(original));
}