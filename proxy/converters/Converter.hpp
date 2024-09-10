#pragma once

#include <Geode/Geode.hpp>
#include "../../lib/json.hpp"
#include "../Enums.hpp"

namespace proxy::converters {
    bool isInt(const std::string& str);
    bool isNumber(const std::string& str);
    bool isBool(const std::string& str);
    bool isNull(const std::string& str);
    bool isString(const std::string& str);
    bool shouldSanitize(const std::string& key);
    std::string safeDump(const nlohmann::json& json, const size_t indent = 2, const bool quoteless = false);
    nlohmann::json getPrimitiveJsonType(const std::string& key, const std::string& str);

    class Converter {
    public:
        virtual enums::ContentType resultContentType() const = 0;
        virtual bool needsSanitization() const = 0;
        virtual bool canConvert(const std::string& path, const bool isBody, const std::string& original) const = 0;
        virtual std::string convert(const std::string& path, const std::string& original) const = 0;
        virtual std::string toRaw(const std::string& path, const std::string& original) const = 0;
    };
}