#pragma once

#include "JsonConverter.hpp"

struct FormToJson : public JsonConverter {
    bool canConvert(const std::string& path, const std::string& original) override;
    json convert(const std::string& path, const std::string& original) override;
};