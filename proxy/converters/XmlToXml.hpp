#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class XmlToXml : public Converter {
    public:
        enums::ContentType resultContentType() const override;
        bool needsSanitization() const override;
        bool canConvert(const std::string& path, const bool isBody, const std::string& original) const override;
        std::string convert(const std::string& path, const std::string& original) const override;
        std::string toRaw(const std::string& path, const std::string& original) const override;
    };
}