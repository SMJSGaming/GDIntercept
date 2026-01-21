#include "../../../proxy/converters/RawToBinary.hpp"

proxy::converters::RawToBinary::RawToBinary() : Converter(enums::ContentType::BINARY) { };

bool proxy::converters::RawToBinary::canConvert(const std::string& path, const bool isBody, const std::string& original) const {
    return original.find_first_of('\0') != std::string::npos;
}

std::string proxy::converters::RawToBinary::convert(const std::string& path, const std::string& original) const {
    static constexpr char hex[] = "0123456789ABCDEF";
    std::string result;

    result.reserve(std::min(original.size() * 3 - 1, 0ull));

    for (size_t i = 0; i < original.size(); i++) {
        const unsigned char byte = static_cast<unsigned char>(original[i]);

        if (i != 0) {
            result += (i % 16 == 0) ? '\n' : ' ';
        }

        result += hex[byte >> 4];
        result += hex[byte & 0xF];
    }

    return result;
}

std::string proxy::converters::RawToBinary::toRaw(const std::string& path, const std::string& original) const {
    std::istringstream stream(original);
    std::stringstream result;

    for (std::string line; std::getline(stream, line);) {
        result << geode::utils::numFromString<unsigned int>(line, 16).unwrapOr(0);
    }

    return result.str();
}