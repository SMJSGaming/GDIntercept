#include "../../proxy/URL.hpp"

using namespace nlohmann;
using namespace proxy::enums;
using namespace geode::prelude;

proxy::URL::URL(std::string url, web::WebRequest* request) : m_original(std::move(url)), m_reconstruction(m_original), m_query(json::object()) {
    if (request != nullptr) {
        const std::unordered_map<std::string, std::string> params = request->getUrlParams();

        if (params.size() && m_reconstruction.find('?') == std::string::npos) {
            m_reconstruction.push_back('?');
        }

        for (const auto& [key, value] : params) {
            m_reconstruction.append(key);
            m_reconstruction.push_back('=');
            m_reconstruction.append(value);
            m_reconstruction.push_back('&');
        }

        if (m_reconstruction.ends_with('&')) {
            m_reconstruction.pop_back();
        }
    }

    this->parse();
}

std::string proxy::URL::getBasicUrl() const {
    return fmt::format("{}://{}/{}", m_protocol, this->getHost(), m_path);
}

std::string proxy::URL::stringifyQuery(const bool raw) const {
    return converters::safeDump(m_query, raw ? -1 : 2);
}

void proxy::URL::parse() {
    size_t index = 0;

    this->parseProtocol(index);
    this->parseLogin(index);

    if (m_scheme == Protocol::HTTP || m_scheme == Protocol::HTTPS || m_scheme == Protocol::WS || m_scheme == Protocol::WSS || m_scheme == Protocol::FTP) {
        this->parseHost(index);
        this->determineOrigin();
    } else {
        m_origin = Origin::OTHER;
    }

    this->parsePath(index);
}

void proxy::URL::parseProtocol(size_t& index) {
    static const std::unordered_map<std::string, Protocol> translations {
        {"http", Protocol::HTTP},
        {"https", Protocol::HTTPS},
        {"ws", Protocol::WS},
        {"wss", Protocol::WSS},
        {"ftp", Protocol::FTP},
        {"file", Protocol::FILE},
        {"steam", Protocol::STEAM},
        {"mailto", Protocol::MAIL},
    };
    const size_t protocolEnd = m_reconstruction.find(":", index);
    const size_t size = protocolEnd - index;

    if (protocolEnd == std::string::npos || (
        m_protocol = m_reconstruction.substr(index, size)
    ).find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789") != std::string::npos) {
        m_scheme = Protocol::UNKNOWN_PROTOCOL;
        m_protocol = "unknown";

        return;
    } else if (translations.contains(m_protocol)) {
        m_scheme = translations.at(m_protocol);
    } else {
        m_scheme = Protocol::UNKNOWN_PROTOCOL;
    }

    std::transform(m_protocol.begin(), m_protocol.end(), m_protocol.begin(), toupper);

    index += size + 1 + m_reconstruction.substr(protocolEnd + 1).starts_with("//") * 2;
}

void proxy::URL::parseLogin(size_t& index) {
    const size_t loginEnd = m_reconstruction.find('@', index);
    const size_t size = loginEnd - index;
    const std::string login = m_reconstruction.substr(index, size);

    if (loginEnd != std::string::npos && login.find('/') == std::string::npos) {
        const size_t split = login.find(':');

        m_username = login.substr(0, split);

        if (split != std::string::npos) {
            m_password = login.substr(split + 1);
        }

        index += size + 1;
    }
}

void proxy::URL::parseHost(size_t& index) {
    const size_t hostEnd = m_reconstruction.find('/', index);
    const size_t size = (hostEnd == std::string::npos ? m_reconstruction.size() : hostEnd) - index;

    m_subDomains = StringStream::of(m_host = m_reconstruction.substr(index, size), '.');

    std::string& lastSubDomain = m_subDomains.back();
    const size_t portStart = lastSubDomain.find(':');

    m_port = m_scheme == Protocol::HTTPS ? 443 : 80;

    if (portStart != std::string::npos) {
        m_port = utils::numFromString<unsigned int>(lastSubDomain.substr(portStart + 1)).unwrapOr(m_port);
        lastSubDomain = lastSubDomain.substr(0, portStart);
    }

    if (lastSubDomain == "localhost") {
        m_domain = m_domainName = lastSubDomain;
        m_subDomains.pop_back();
    } else for (size_t i = m_subDomains.size() - 1; i >= 0; i--) {
        const std::string& subDomain = m_subDomains[i];

        m_domain = subDomain + (m_domain.empty() ? m_domain : '.' + m_domain);

        if (m_tld.empty()) {
            m_tld = subDomain;
        } else if (m_domain.find('.') != std::string::npos) {
            m_domainName = subDomain;

            m_subDomains.pop_back();
            break;
        }

        m_subDomains.pop_back();
    }

    index += size;
}

void proxy::URL::parsePath(size_t& index) {
    converters::FormToJson converter;
    std::string fullPath = m_reconstruction.substr(index);
    const size_t size = fullPath.size();
    const size_t queryStart = fullPath.find('?');
    const size_t hashStart = fullPath.find('#');
    const size_t pathEnd = queryStart == std::string::npos ? hashStart == std::string::npos ? size : hashStart : queryStart;

    m_path = pathEnd == 0 ? "/" : fullPath.substr(0, pathEnd);

    if (hashStart != std::string::npos) {
        m_hash = fullPath.substr(hashStart + 1);
        fullPath = fullPath.substr(0, hashStart);
    }

    if (queryStart != std::string::npos && converter.canConvert(m_path, false, m_queryString = fullPath.substr(queryStart + 1))) {
        m_query = json::parse(converter.convert(m_path, m_queryString));
    }

    index += size;
}

void proxy::URL::determineOrigin() {
    if (m_domain == "boomlings.com" || m_domain == "geometrydash.com") {
        m_origin = Origin::GD;
    } else if (m_domain == "geometrydashfiles.b-cdn.net" || m_domain == "geometrydashcontent.b-cdn.net") {
        m_origin = Origin::GD_CDN;
    // robtop.games is currently in the process of being sold
    } else if (
        m_domain == "robtopgames.org" ||
        m_domain == "robtopgames.net" ||
        m_domain == "robtopgames.com" ||
        m_domain == "robtop.games"
    ) {
        m_origin = Origin::ROBTOP_GAMES;
    } else if (m_domain == "ngfiles.com") {
        m_origin = Origin::NEWGROUNDS;
    } else if (m_domain == "geode-sdk.org") {
        m_origin = Origin::GEODE;
    } else if (m_domain == "localhost") {
        m_origin = Origin::LOCALHOST;
    } else {
        m_origin = Origin::OTHER;
    }
}