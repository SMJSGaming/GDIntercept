#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class FormToJson : public Converter<nlohmann::json> {
    public:
        bool canConvert(const std::string& path, const std::string& original) override;
        nlohmann::json convert(const std::string& path, const std::string& original) override;
    };
}