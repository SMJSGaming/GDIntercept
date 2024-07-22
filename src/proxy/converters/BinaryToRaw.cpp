#include "../../../proxy/converters/BinaryToRaw.hpp"

bool proxy::converters::BinaryToRaw::canConvert(const std::string& path, const std::string& original) {
    return original.find_first_of('\0') != std::string::npos;
}

std::string proxy::converters::BinaryToRaw::convert(const std::string& path, const std::string& original) {
    std::stringstream result;

    result << std::hex << std::setfill('0') << std::uppercase;

    // This way we avoid having to substr the first character
    if (original.size()) {
        result << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(original[0]));
    }

    for (size_t i = 1; i < original.size(); i++) {
        result << (i % 16 == 0 ? '\n' : ' ') << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(original[i]));
    }

    return result.str();
}