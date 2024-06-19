#pragma once

#include "../../include.hpp"

struct JsonConverter {
    static bool isInt(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isBool(const std::string& str);
    static bool isNull(const std::string& str);
    static bool isString(const std::string& str);
    static bool isJson(const std::string& str);
    static json getPrimitiveType(const std::string& key, const std::string& str);

    virtual bool canConvert(const std::string& path, const std::string& original) = 0;
    virtual json convert(const std::string& path, const std::string& original) = 0;
};