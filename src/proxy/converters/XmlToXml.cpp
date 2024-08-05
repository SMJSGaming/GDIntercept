#include "../../../proxy/converters/XmlToXml.hpp"

proxy::enums::ContentType proxy::converters::XmlToXml::resultContentType() const {
    return enums::ContentType::XML;
}

bool proxy::converters::XmlToXml::needsSanitization() const {
    return false;
}

bool proxy::converters::XmlToXml::canConvert(const std::string& path, const bool isBody, const std::string& original) const {
    return original.starts_with("<?xml") ||
        original.starts_with("<!DOCTYPE") ||
        original.starts_with("<html");
}

std::string proxy::converters::XmlToXml::convert(const std::string& path, const std::string& original) const {
    return original;
}

std::string proxy::converters::XmlToXml::toRaw(const std::string& path, const std::string& original) const {
    return original;
}
