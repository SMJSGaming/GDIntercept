#include "../../../proxy/converters/RawToBinary.hpp"

proxy::converters::RawToBinary::RawToBinary() : Converter(enums::ContentType::BINARY) { };

bool proxy::converters::RawToBinary::canConvert(const std::string& path, const bool isBody, const std::string& original) const {
    return original.find_first_of('\0') != std::string::npos;
}

std::string proxy::converters::RawToBinary::convert(const std::string& path, const std::string& original) const {
    std::stringstream result;

    result << std::hex << std::setfill('0') << std::uppercase;

    // This way we avoid having to substr the first character
    if (original.size()) {
        result << std::setw(2) << static_cast<unsigned int>(original[0]);
    }

    for (size_t i = 1; i < original.size(); i++) {
        result << (i % 16 == 0 ? '\n' : ' ') << std::setw(2) << static_cast<unsigned int>(original[i]);
    }

    return result.str();
}

std::string proxy::converters::RawToBinary::toRaw(const std::string& path, const std::string& original) const {
    std::istringstream stream(original);
    std::stringstream result;
    std::string line;

    while (std::getline(stream, line)) {
        result << geode::utils::numFromString<unsigned int>(line, 16).unwrapOr(0);
    }

    return result.str();
}