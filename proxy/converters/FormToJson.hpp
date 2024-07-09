#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class FormToJson : public Converter<json> {
    public:
        bool canConvert(const std::string& path, const std::string& original) override;
        json convert(const std::string& path, const std::string& original) override;
    };
}