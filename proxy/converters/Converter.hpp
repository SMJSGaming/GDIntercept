#pragma once

#include <Geode/Geode.hpp>
#include "../../lib/json.hpp"

namespace proxy::converters {
    bool isInt(const std::string& str);
    bool isNumber(const std::string& str);
    bool isBool(const std::string& str);
    bool isNull(const std::string& str);
    bool isString(const std::string& str);
    bool isJson(const std::string& str);
    nlohmann::json getPrimitiveJsonType(const std::string& key, const std::string& str);

    template<typename T>
    class Converter {
    public:
        virtual bool canConvert(const std::string& path, const std::string& original) = 0;
        virtual T convert(const std::string& path, const std::string& original) = 0;
    };
}