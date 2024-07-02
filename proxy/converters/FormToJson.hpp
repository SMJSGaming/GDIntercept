#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    struct FormToJson : public Converter<json> {
        bool canConvert(const std::string& path, const std::string& original) override;
        json convert(const std::string& path, const std::string& original) override;
    };
}