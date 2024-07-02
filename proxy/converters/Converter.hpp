#pragma once

#include <Geode/Geode.hpp>
#include "../../lib/json.hpp"

namespace proxy::converters {
    using namespace geode::prelude;
    using namespace nlohmann;

    bool isInt(const std::string& str);
    bool isNumber(const std::string& str);
    bool isBool(const std::string& str);
    bool isNull(const std::string& str);
    bool isString(const std::string& str);
    bool isJson(const std::string& str);
    json getPrimitiveJsonType(const std::string& key, const std::string& str);

    template<typename T>
    struct Converter {
        virtual bool canConvert(const std::string& path, const std::string& original) = 0;
        virtual T convert(const std::string& path, const std::string& original) = 0;
    };
}