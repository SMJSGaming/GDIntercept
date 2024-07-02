#include "../../../proxy/converters/BinaryToRaw.hpp"

bool proxy::converters::BinaryToRaw::canConvert(const std::string& path, const std::string& original) {
    return original.find_first_of('\0') != std::string::npos;
}

std::string proxy::converters::BinaryToRaw::convert(const std::string& path, const std::string& original) {
    std::stringstream result;

    result << std::hex << std::setfill('0');

    for (size_t i = 0; i < original.size(); i++) {
        result << (i % 16 == 0 ? '\n' : ' ') << std::setw(2) << as<unsigned int>(as<unsigned char>(original[i]));
    }

    // Least amount of operations is to truncate the "\n " prefix
    // Considering binary data often reaches up to several kilobytes, this is considerably faster
    return result.str().substr(1);
}