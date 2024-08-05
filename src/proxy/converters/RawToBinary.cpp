#include "../../../proxy/converters/RawToBinary.hpp"

proxy::enums::ContentType proxy::converters::RawToBinary::resultContentType() const {
    return enums::ContentType::BINARY;
}

bool proxy::converters::RawToBinary::needsSanitization() const {
    return false;
}

bool proxy::converters::RawToBinary::canConvert(const std::string& path, const bool isBody, const std::string& original) const {
    return original.find_first_of('\0') != std::string::npos;
}

std::string proxy::converters::RawToBinary::convert(const std::string& path, const std::string& original) const {
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

std::string proxy::converters::RawToBinary::toRaw(const std::string& path, const std::string& original) const {
    std::istringstream stream(original);
    std::stringstream result;
    std::string line;

    while (std::getline(stream, line)) {
        result << static_cast<unsigned char>(std::stoi(line, nullptr, 16));
    }

    return result.str();
}