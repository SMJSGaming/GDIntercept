#include "../../proxy/HttpInfo.hpp"

proxy::converters::FormToJson proxy::HttpInfo::formToJson;

proxy::converters::RobTopToJson proxy::HttpInfo::robtopToJson;

proxy::converters::BinaryToRaw proxy::HttpInfo::binaryToRaw;

proxy::HttpInfo::HttpInfo(CCHttpRequest* request) : m_paused(this->shouldPause()),
    m_method(request->getRequestType()),
    m_url(request->getUrl()),
    m_query(json::object()),
    m_headers(json::object()),
    m_statusCode(0),
    m_responseContentType(ContentType::UNKNOWN_CONTENT) {
    const char* body = request->getRequestData();

    this->resetCache();
    this->parseUrl();
    this->determineOrigin();

    // CCHttpRequest headers technically allow for weird header formats, but I'm assuming they're all key-value pairs separated by a colon since this is the standard
    // If you don't follow the standard, I'm not going to care that you get shitty results
    for (const gd::string& header : request->getHeaders()) {
        const std::string headerStr(header.c_str());
        const size_t colonPos = headerStr.find(":");

        if (colonPos == std::string::npos) {
            m_headers[headerStr] = json();
        } else {
            std::string value(headerStr.substr(colonPos + 1));

            m_headers[headerStr.substr(0, colonPos)] = json(value.erase(0, value.find_first_not_of(' ')));
        }
    }

    m_body = body ? std::string(request->getRequestData()).substr(0, request->getRequestDataSize()) : "";
    m_bodyContentType = this->determineContentType(m_body, true);
}

proxy::HttpInfo::HttpInfo(web::WebRequest* request, const std::string& method, const std::string& url) : m_paused(this->shouldPause()),
    m_url(url),
    m_query(json::object()),
    m_headers(json::object()),
    m_statusCode(0),
    m_responseContentType(ContentType::UNKNOWN_CONTENT) {
    // this->resetCache();
    // this->parseUrl(url);

    // for (const auto& [key, value] : request->m_impl->headers()) {
    //     m_headers[key] = json(value);
    // }

    // m_body = request->m_impl->body();
    // m_bodyContentType = this->determineContentType(m_body, true);
}

std::string proxy::HttpInfo::stringifyProtocol() const {
    switch (m_protocol) {
        case Protocol::HTTP: return "HTTP";
        case Protocol::HTTPS: return "HTTPS";
        default: return "UNKNOWN";
    }
}

std::string proxy::HttpInfo::stringifyMethod() const {
    switch (m_method) {
        case CCHttpRequest::kHttpGet: return "GET";
        case CCHttpRequest::kHttpPost: return "POST";
        case CCHttpRequest::kHttpPut: return "PUT";
        case CCHttpRequest::kHttpDelete: return "DELETE";
        default: return "UNKNOWN";
    }
}

std::string proxy::HttpInfo::stringifyQuery() const {
    return m_query.dump(2);
}

std::string proxy::HttpInfo::stringifyHeaders() const {
    return m_headers.dump(2);
}

std::string proxy::HttpInfo::stringifyStatusCode() const {
    switch (m_statusCode) {
        case -1: return "Request Error";
        case 0: return "No Response";
        default: return std::to_string(m_statusCode);
    }
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::getBodyContent(const bool raw) {
    return this->getContent(raw, m_bodyContentType, m_body, m_simplifiedBodyCache);
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::getResponseContent(const bool raw) {
    return this->getContent(raw, m_responseContentType, m_response, m_simplifiedResponseCache);
}

bool proxy::HttpInfo::isPaused() const {
    return m_paused;
}

void proxy::HttpInfo::resetCache() {
    m_simplifiedBodyCache = { ContentType::UNKNOWN_CONTENT, "" };
    m_simplifiedResponseCache = { ContentType::UNKNOWN_CONTENT, "" };
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::getContent(const bool raw, const ContentType originalContentType, const std::string& original, HttpContent& cache) {
    if (raw) {
        return { originalContentType, original };
    } else if (cache.contents.empty()) {
        const HttpContent simplified = this->simplifyContent({ originalContentType, original });

        if (Mod::get()->getSettingValue<bool>("cache")) {
            cache = simplified;
        }

        return simplified;
    } else {
        return cache;
    }
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::simplifyContent(const HttpContent& content) {
    switch (content.type) {
        case ContentType::FORM: return {
            ContentType::JSON,
            HttpInfo::formToJson.convert(m_path, content.contents).dump(2)
        };
        case ContentType::ROBTOP: return {
            ContentType::JSON,
            HttpInfo::robtopToJson.convert(m_path, content.contents).dump(2)
        };
        case ContentType::BINARY: return {
            ContentType::BINARY,
            HttpInfo::binaryToRaw.convert(m_path, content.contents)
        };
        default: return content;
    }
}

proxy::HttpInfo::ContentType proxy::HttpInfo::determineContentType(const std::string& content, const bool isBody) {
    if (content.empty()) {
        return ContentType::UNKNOWN_CONTENT;
    } else if (HttpInfo::binaryToRaw.canConvert(m_path, content)) {
        return ContentType::BINARY;
    } else if (converters::isJson(content)) {
        return ContentType::JSON;
    } else if (content.starts_with('<')) {
        return ContentType::XML;
    } else if (!isBody && HttpInfo::robtopToJson.canConvert(m_path, content)) {
        return ContentType::ROBTOP;
    } else if (HttpInfo::formToJson.canConvert(m_path, content)) {
        return ContentType::FORM;
    } else {
        return ContentType::UNKNOWN_CONTENT;
    }
}

bool proxy::HttpInfo::isDomain(const std::string& domain) {
    return m_host == domain || m_host.ends_with("." + domain);
}

bool proxy::HttpInfo::shouldPause() {
    return Mod::get()->getSettingValue<bool>("pause-requests");
}

void proxy::HttpInfo::determineOrigin() {
    if (this->isDomain("boomlings.com") || this->isDomain("geometrydash.com")) {
        m_origin = Origin::GD;
    } else if (this->isDomain("geometrydashfiles.b-cdn.net") || this->isDomain("geometrydashcontent.b-cdn.net")) {
        m_origin = Origin::GD_CDN;
    // robtop.games is currently in the process of being sold
    } else if (
        this->isDomain("robtopgames.org") ||
        this->isDomain("robtopgames.net") ||
        this->isDomain("robtopgames.com") ||
        this->isDomain("robtop.games")
    ) {
        m_origin = Origin::ROBTOP_GAMES;
    } else if (this->isDomain("ngfiles.com")) {
        m_origin = Origin::NEWGROUNDS;
    } else if (this->isDomain("geode-sdk.org")) {
        m_origin = Origin::GEODE;
    } else {
        m_origin = Origin::OTHER;
    }
}

void proxy::HttpInfo::parseUrl() {
    const size_t protocolEnd = m_url.find("://");
    const size_t queryStart = m_url.find('?');
    size_t pathStart;

    if (protocolEnd == std::string::npos) {
        pathStart = m_url.find('/');
        m_protocol = Protocol::UNKNOWN_PROTOCOL;
        m_host = m_url.substr(0, pathStart == std::string::npos ? queryStart : pathStart);
    } else {
        const std::string protocol(m_url.substr(0, protocolEnd));

        if (protocol == "https") {
            m_protocol = Protocol::HTTPS;
        } else if (protocol == "http") {
            m_protocol = Protocol::HTTP;
        } else {
            m_protocol = Protocol::UNKNOWN_PROTOCOL;
        }

        pathStart = m_url.find('/', protocolEnd + 3);
        m_host = m_url.substr(protocolEnd + 3, (pathStart == std::string::npos ? queryStart : pathStart) - protocolEnd - 3);
    }

    if (pathStart == std::string::npos) {
        m_path = "/";
    } else {
        m_path = m_url.substr(pathStart, queryStart == std::string::npos ? std::string::npos : queryStart - pathStart);
    }

    if (queryStart != std::string::npos) {
        m_query = HttpInfo::formToJson.convert(m_path, m_url.substr(queryStart + 1));
    }
}