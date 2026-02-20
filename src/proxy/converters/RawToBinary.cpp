#include "RawToBinary.hpp"

proxy::converters::RawToBinary::RawToBinary() : Converter(enums::ContentType::BINARY) { };

bool proxy::converters::RawToBinary::canConvert(const std::string_view path, const bool isBody, const std::string_view original) const {
    return original.find_first_of('\0') != std::string_view::npos;
}

std::string proxy::converters::RawToBinary::convert(const std::string_view path, const std::string_view original) const {
    static constexpr char hex[] = "0123456789ABCDEF";
    std::string result;

    if (original.empty()) {
        return result;
    }

    result.reserve(original.size() * 3 - 1);

    for (size_t i = 0; i < original.size(); i++) {
        const unsigned char byte = static_cast<unsigned char>(original[i]);

        if (i != 0) {
            result.push_back((i % 16 == 0) ? '\n' : ' ');
        }

        result.push_back(hex[byte >> 4]);
        result.push_back(hex[byte & 0xF]);
    }

    return result;
}

std::string proxy::converters::RawToBinary::toRaw(const std::string_view path, const std::string_view original) const {
    std::string result;

    result.reserve(original.size() / 3 + 1);

    for (size_t i = 0; (i + 1) < original.size(); i += 3) {
        result.push_back((this->hexValue(original[i]) << 4) | this->hexValue(original[i + 1]));
    }

    return result;
}