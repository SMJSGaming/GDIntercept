#include "../../proxy/HttpInfo.hpp"

static size_t globalIndexCounter = 1;

proxy::converters::FormToJson proxy::HttpInfo::formToJson;

proxy::converters::RobTopToJson proxy::HttpInfo::robtopToJson;

proxy::converters::BinaryToRaw proxy::HttpInfo::binaryToRaw;

proxy::HttpInfo::HttpContent proxy::HttpInfo::getContent(const bool raw, const ContentType originalContentType, const std::string& path, const std::string& original, HttpContent& cache) {
    if (raw) {
        return { originalContentType, original };
    } else if (cache.contents.empty()) {
        const HttpContent simplified = HttpInfo::simplifyContent(path, { originalContentType, original });

        if (Mod::get()->getSettingValue<bool>("cache")) {
            cache = simplified;
        }

        return simplified;
    } else {
        return cache;
    }
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::simplifyContent(const std::string& path, const HttpContent& content) {
    switch (content.type) {
        case ContentType::JSON: return {
            ContentType::JSON,
            json::parse(content.contents).dump(2)
        };
        case ContentType::FORM: return {
            ContentType::JSON,
            HttpInfo::formToJson.convert(path, content.contents).dump(2)
        };
        case ContentType::ROBTOP: return {
            ContentType::JSON,
            HttpInfo::robtopToJson.convert(path, content.contents).dump(2)
        };
        case ContentType::BINARY: return {
            ContentType::BINARY,
            HttpInfo::binaryToRaw.convert(path, content.contents)
        };
        default: return content;
    }
}

proxy::HttpInfo::ContentType proxy::HttpInfo::determineContentType(const std::string& path, const std::string& content, const bool isBody) {
    if (content.empty()) {
        return ContentType::UNKNOWN_CONTENT;
    } else if (HttpInfo::binaryToRaw.canConvert(path, content)) {
        return ContentType::BINARY;
    } else if (converters::isJson(content)) {
        return ContentType::JSON;
    } else if (content.starts_with('<')) {
        return ContentType::XML;
    } else if (!isBody && HttpInfo::robtopToJson.canConvert(path, content)) {
        return ContentType::ROBTOP;
    } else if (HttpInfo::formToJson.canConvert(path, content)) {
        return ContentType::FORM;
    } else {
        return ContentType::UNKNOWN_CONTENT;
    }
}

nlohmann::json proxy::HttpInfo::parseCocosHeaders(const gd::vector<char>* headers) {
    std::stringstream headerStream(std::string(headers->begin(), headers->end()));
    gd::vector<gd::string> headerStrings;

    for (gd::string header; std::getline(headerStream, header, '\n');) {
        headerStrings.push_back(header);
    }

    return HttpInfo::parseCocosHeaders(headerStrings);
}

nlohmann::json proxy::HttpInfo::parseCocosHeaders(const gd::vector<gd::string>& headers) {
    json parsed = json::object();

     // CCHttp headers technically allow for weird header formats, but I'm assuming they're all key-value pairs separated by a colon since this is the standard
    // If you don't follow the standard, I'm not going to care that you get shitty results
    for (const gd::string& header : headers) {
        const std::string headerString(header.c_str());
        const size_t colonPos = headerString.find(":");

        if (colonPos == std::string::npos) {
            parsed[headerString] = json();
        } else {
            std::string value(headerString.substr(colonPos + 1));

            parsed[headerString.substr(0, colonPos)] = json(value.erase(0, value.find_first_not_of(' ')));
        }
    }

    return parsed;
}

bool proxy::HttpInfo::shouldPause() {
    return Mod::get()->getSettingValue<bool>("pause-requests");
}

proxy::HttpInfo::HttpInfo(CCHttpRequest* request) : m_id(globalIndexCounter++),
m_paused(this->shouldPause()),
m_request(request) { }

proxy::HttpInfo::HttpInfo(web::WebRequest* request, const std::string& method, const std::string& url) : m_id(globalIndexCounter++),
m_paused(this->shouldPause()),
m_request(request, method, url) { }

bool proxy::HttpInfo::isPaused() const {
    return m_paused;
}

bool proxy::HttpInfo::responseReceived() const {
    return m_response.received();
}

void proxy::HttpInfo::resetCache() {
    m_request.resetCache();

    if (this->responseReceived()) {
        m_response.resetCache();
    }
}

void proxy::HttpInfo::resume() {
    m_paused = false;
}

std::string proxy::HttpInfo::URL::stringifyMethod(const CCHttpRequest::HttpRequestType method) {
    switch (method) {
        case CCHttpRequest::kHttpGet: return "GET";
        case CCHttpRequest::kHttpPost: return "POST";
        case CCHttpRequest::kHttpPut: return "PUT";
        case CCHttpRequest::kHttpDelete: return "DELETE";
        default: return "UNKNOWN";
    }
}

proxy::HttpInfo::Origin proxy::HttpInfo::URL::determineOrigin(const std::string& host) {
    #define isDomain(domain) host == std::string(domain) || host.ends_with("." + std::string(domain))

    if (isDomain("boomlings.com") || isDomain("geometrydash.com")) {
        return Origin::GD;
    } else if (isDomain("geometrydashfiles.b-cdn.net") || isDomain("geometrydashcontent.b-cdn.net")) {
        return Origin::GD_CDN;
    // robtop.games is currently in the process of being sold
    } else if (
        isDomain("robtopgames.org") ||
        isDomain("robtopgames.net") ||
        isDomain("robtopgames.com") ||
        isDomain("robtop.games")
    ) {
        return Origin::ROBTOP_GAMES;
    } else if (isDomain("ngfiles.com")) {
        return Origin::NEWGROUNDS;
    } else if (isDomain("geode-sdk.org")) {
        return Origin::GEODE;
    } else {
        return Origin::OTHER;
    }
}

proxy::HttpInfo::URL::URL(CCHttpRequest* request) : m_method(URL::stringifyMethod(request->getRequestType())),
m_raw(request->getUrl()),
m_query(json::object()) {
    this->parse();
}

proxy::HttpInfo::URL::URL(web::WebRequest* request, const std::string& method, const std::string& url) : m_method(method),
m_raw(url),
m_query(json::object()) {
    const std::unordered_map<std::string, std::string> params = request->getUrlParams();

    if (params.size() && m_raw.find('?') == std::string::npos) {
        m_raw += "?";
    }

    for (const auto& [key, value] : params) {
        m_raw += key + '=' + value + '&';
    }

    if (m_raw.ends_with('&')) {
        m_raw.pop_back();
    }

    this->parse();
}

std::string proxy::HttpInfo::URL::stringifyProtocol() const {
    switch (m_protocol) {
        case Protocol::HTTP: return "HTTP";
        case Protocol::HTTPS: return "HTTPS";
        default: return "UNKNOWN";
    }
}

std::string proxy::HttpInfo::URL::stringifyQuery() const {
    return m_query.dump(2);
}

void proxy::HttpInfo::URL::parse() {
    const size_t protocolEnd = m_raw.find("://");
    const size_t queryStart = m_raw.find('?');
    size_t pathStart;

    if (protocolEnd == std::string::npos) {
        pathStart = m_raw.find('/');
        m_protocol = Protocol::UNKNOWN_PROTOCOL;
        m_host = m_raw.substr(0, pathStart == std::string::npos ? queryStart : pathStart);
    } else {
        const std::string protocol(m_raw.substr(0, protocolEnd));

        if (protocol == "https") {
            m_protocol = Protocol::HTTPS;
        } else if (protocol == "http") {
            m_protocol = Protocol::HTTP;
        } else {
            m_protocol = Protocol::UNKNOWN_PROTOCOL;
        }

        pathStart = m_raw.find('/', protocolEnd + 3);
        m_host = m_raw.substr(protocolEnd + 3, (pathStart == std::string::npos ? queryStart : pathStart) - protocolEnd - 3);
    }

    if (pathStart == std::string::npos) {
        m_path = "/";
    } else {
        m_path = m_raw.substr(pathStart, queryStart == std::string::npos ? std::string::npos : queryStart - pathStart);
    }

    if (queryStart != std::string::npos) {
        m_query = HttpInfo::formToJson.convert(m_path, m_raw.substr(queryStart + 1));
    }

    m_origin = URL::determineOrigin(m_host);
}

proxy::HttpInfo::Request::Request(CCHttpRequest* request) : m_url(request), 
m_headers(HttpInfo::parseCocosHeaders(request->getHeaders())),
m_body(std::string(request->getRequestData(), request->getRequestDataSize())),
m_contentType(HttpInfo::determineContentType(m_url.getPath(), m_body, true)),
m_simplifiedBodyCache({ ContentType::UNKNOWN_CONTENT, "" }) { }

proxy::HttpInfo::Request::Request(web::WebRequest* request, const std::string& method, const std::string& url) : m_url(request, method, url),
m_headers(json::object()),
m_simplifiedBodyCache({ ContentType::UNKNOWN_CONTENT, "" }) {
    const ByteVector body = request->getBody().value_or(ByteVector());

    for (const auto& [key, value] : request->getHeaders()) {
        m_headers[key] = json(value);
    }

    m_body = std::string(body.begin(), body.end());
    m_contentType = HttpInfo::determineContentType(m_url.getPath(), m_body, true);
}

std::string proxy::HttpInfo::Request::stringifyHeaders() const {
    return m_headers.dump(2);
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::Request::getBodyContent() const {
    return { m_contentType, m_body };
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::Request::getBodyContent(const bool raw) {
    return HttpInfo::getContent(raw, m_contentType, m_url.getPath(), m_body, m_simplifiedBodyCache);
}

void proxy::HttpInfo::Request::resetCache() {
    m_simplifiedBodyCache = { ContentType::UNKNOWN_CONTENT, "" };
}

proxy::HttpInfo::Response::Response() : m_received(false) { };

proxy::HttpInfo::Response::Response(Request* request, CCHttpResponse* response) : m_received(true),
m_request(request),
m_headers(HttpInfo::parseCocosHeaders(response->getResponseHeader())),
m_statusCode(response->getResponseCode()),
m_simplifiedResponseCache({ ContentType::UNKNOWN_CONTENT, "" }) {
    gd::vector<char>* data = response->getResponseData();

    m_response = std::string(data->begin(), data->end());
    m_contentType = HttpInfo::determineContentType(request->m_url.getPath(), m_response);
}

proxy::HttpInfo::Response::Response(Request* request, web::WebResponse* response) : m_received(true),
m_request(request),
m_headers(json::object()),
m_statusCode(response->code()),
m_response(response->string().unwrapOrDefault()),
m_contentType(HttpInfo::determineContentType(request->m_url.getPath(), m_response)),
m_simplifiedResponseCache({ ContentType::UNKNOWN_CONTENT, "" }) {
    for (const std::string& key : response->headers()) {
        m_headers[key] = json(response->header(key).value_or(""));
    }
}

std::string proxy::HttpInfo::Response::stringifyHeaders() const {
    return m_headers.dump(2);
}

std::string proxy::HttpInfo::Response::stringifyStatusCode() const {
    switch (m_statusCode) {
        case -1: return "Request Error";
        default: return std::to_string(m_statusCode);
    }
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::Response::getResponseContent() const {
    return { m_contentType, m_response };
}

proxy::HttpInfo::HttpContent proxy::HttpInfo::Response::getResponseContent(const bool raw) {
    return HttpInfo::getContent(raw, m_contentType, m_request->getURL().getPath(), m_response, m_simplifiedResponseCache);
}

bool proxy::HttpInfo::Response::received() const {
    return m_received;
}

void proxy::HttpInfo::Response::resetCache() {
    m_simplifiedResponseCache = { ContentType::UNKNOWN_CONTENT, "" };
}