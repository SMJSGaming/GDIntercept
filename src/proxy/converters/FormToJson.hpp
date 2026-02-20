#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class FormToJson : public Converter {
    public:
        FormToJson();
        bool canConvert(const std::string_view path, const bool isBody, const std::string_view original) const override;
        std::string convert(const std::string_view path, const std::string_view original) const override;
        std::string toRaw(const std::string_view path, const std::string_view original) const override;
    };
}