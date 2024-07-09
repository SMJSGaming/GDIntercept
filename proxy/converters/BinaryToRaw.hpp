#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    // What can I say? I like it raw
    class BinaryToRaw : public Converter<std::string> {
    public:
        bool canConvert(const std::string& path, const std::string& original) override;
        std::string convert(const std::string& path, const std::string& original) override;
    };
}