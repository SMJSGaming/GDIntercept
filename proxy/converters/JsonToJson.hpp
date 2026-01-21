#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class JsonToJson : public Converter {
    public:
        JsonToJson();
        bool canConvert(const std::string& path, const bool isBody, const std::string& original) const override;
        std::string convert(const std::string& path, const std::string& original) const override;
        std::string toRaw(const std::string& path, const std::string& original) const override;
    private:
        void recursiveSanitize(nlohmann::json& object) const;
    };
}