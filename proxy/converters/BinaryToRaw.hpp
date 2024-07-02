#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    // What can I say? I like it raw
    struct BinaryToRaw : public Converter<std::string> {
        bool canConvert(const std::string& path, const std::string& original) override;
        std::string convert(const std::string& path, const std::string& original) override;
    };
}