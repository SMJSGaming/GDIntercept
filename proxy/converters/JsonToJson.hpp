#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class JsonToJson : public Converter {
    public:
        JsonToJson();
        bool canConvert(const std::string_view path, const bool isBody, const std::string_view original) const override;
        std::string convert(const std::string_view path, const std::string_view original) const override;
        std::string toRaw(const std::string_view path, const std::string_view original) const override;
    private:
        void recursiveSanitize(nlohmann::json& object) const;
    };
}