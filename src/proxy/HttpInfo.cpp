#include "HttpInfo.hpp"
#include "Proxy.hpp"

using namespace nlohmann;
using namespace geode::prelude;
using namespace proxy::prelude;
using namespace web;

static std::atomic_size_t globalIndexCounter = 1;

const LookupTable<ContentType, converters::Converter*> HttpInfo::CONVERTERS({
    { ContentType::BINARY, new RawToBinary() },
    { ContentType::XML, new XmlToXml() },
    { ContentType::JSON, new JsonToJson() },
    { ContentType::ROBTOP, new RobTopToJsonV2() },
    { ContentType::FORM, new FormToJson() }
});

HttpInfo::Content HttpInfo::getContent(const bool raw, const ContentType originalContentType, const std::string_view path, const std::string_view original) {
    if (HttpInfo::CONVERTERS.contains(originalContentType)) {
        const converters::Converter* converter = HttpInfo::CONVERTERS.at(originalContentType);

        if (!raw) {
            return { converter->getResultContentType(), converter->convert(path, original) };
        } else if (Mod::get()->getSettingValue<bool>("censor-data") && converter->getNeedsSanitization()) {
            return { originalContentType, converter->toRaw(path, converter->convert(path, original)) };
        }
    }

    return { originalContentType, std::string(original) };
}

HttpInfo::Content HttpInfo::getHeaders(const bool raw, const Headers& headers) {
    if (raw) {
        std::string rawHeaders;

        for (const auto& [headerKey, list] : headers) {
            for (const std::string_view headerValue : list) {
                rawHeaders.append(headerKey)
                    .append(": ")
                    .append(headerValue)
                    .push_back('\n');
            }
        }

        return { ContentType::UNKNOWN_CONTENT, std::move(rawHeaders) };
    } else {
        ordered_json content = ordered_json::object();

        for (const auto& [headerKey, list] : headers) {
            content[headerKey] = list.size() == 1 ? json(list.at(0)) : json(list);
        }

        return { ContentType::JSON, converters::safeDump(content) };
    }
}

ContentType HttpInfo::determineContentType(const std::string_view path, const bool isBody, const std::string_view content) {
    if (content.empty()) {
        return ContentType::UNKNOWN_CONTENT;
    }

    for (const auto& [type, converter] : HttpInfo::CONVERTERS) {
        if (converter->canConvert(path, isBody, content)) {
            return type;
        }
    }

    return ContentType::UNKNOWN_CONTENT;
}

std::string HttpInfo::translateHttpVersion(const HttpVersion version) {
    static const std::unordered_map<HttpVersion, std::string> translations {
        { HttpVersion::VERSION_1_0, "HTTP/1.0" },
        { HttpVersion::VERSION_1_1, "HTTP/1.1" },
        { HttpVersion::VERSION_2_0, "HTTP/2" },
        { HttpVersion::VERSION_3, "HTTP/3" }
    };

    return translations.contains(version) ? translations.at(version) : "HTTP/2";
}

HttpInfo::Headers HttpInfo::parseCocosHeaders(const gd::vector<char>* headers) {
    return HttpInfo::parseCocosHeaders(StringStream::split(std::string_view(headers->data(), headers->size()), '\n')
        .map([](std::string&& header) {
            if (header.back() == '\r') {
                header.pop_back();
            }

            return gd::string(std::move(header));
        })
        .filter([](const std::string_view header) { return header.size(); }));
}

HttpInfo::Headers HttpInfo::parseCocosHeaders(const gd::vector<gd::string>& headers) {
    Headers parsed;

    for (const std::string_view header : headers) {
        const size_t colonPos = header.find(':');

        if (colonPos != std::string_view::npos) {
            const size_t valueOffset = colonPos + 1;
            const size_t valueStartOffset = header.find_first_not_of(' ', valueOffset);

            parsed[std::string(header.substr(0, colonPos))].emplace_back(header.substr(
                valueStartOffset == std::string_view::npos ? valueOffset : valueStartOffset
            ));
        } else {
            parsed["<GDI_CUSTOM:Keyless>"].emplace_back(header);
        }
    }

    return parsed;
}

HttpInfo::HttpInfo(CCHttpRequest* request) : m_id(globalIndexCounter++),
m_client(Client::COCOS),
m_state(Mod::get()->getSavedValue<bool>("paused") ? State::PAUSED : State::IN_PROGRESS),
m_repeat(false),
m_httpVersion("HTTP/1.1"),
m_url(request->getUrl()),
m_request(m_url, request) { }

HttpInfo::HttpInfo(const bool repeat, const WebRequest& request) : m_id(globalIndexCounter++),
m_client(Client::GEODE),
m_state(State::IN_PROGRESS),
m_repeat(repeat),
m_httpVersion(HttpInfo::translateHttpVersion(request.getHttpVersion())),
m_url(request.getUrl(), request),
m_request(m_url, request) { }

void HttpInfo::cancel() {
    if (m_state == State::IN_PROGRESS || m_state == State::PAUSED) {
        std::shared_ptr<HttpInfo> self = shared_from_this();

        m_state = State::CANCELLED;
        m_response.emplace(Response(self, static_cast<int>(GeodeWebError::REQUEST_CANCELLED)));

        ProxyEvent(EventState::CANCEL).send(self);
    }
}

bool HttpInfo::isPaused() const {
    return m_state == State::PAUSED;
}

bool HttpInfo::isInProgress() const {
    return m_state == State::IN_PROGRESS;
}

bool HttpInfo::isCompleted() const {
    return m_state == State::COMPLETED;
}

bool HttpInfo::isFaulty() const {
    return m_state == State::FAULTY;
}

bool HttpInfo::isCancelled() const {
    return m_state == State::CANCELLED;
}

bool HttpInfo::hasResponse() const {
    return m_response.has_value();
}

std::string HttpInfo::toString(const bool cutContent) const {
    Content bodyContent = m_request.getBodyContent();
    utils::StringBuffer info;
    std::string scheme = m_url.getStringScheme();

    std::transform(scheme.begin(), scheme.end(), scheme.begin(), toupper);

    info.append("Client: {}\nRequest Time: {}.{}\nHTTP Version: {}\nMethod: {}\nScheme: {}\nHost: {}",
        m_client == Client::COCOS ? "Cocos2D-X" : "Geode",
        geode::localtime(m_request.m_startTime / 1000),
        m_request.m_startTime % 1000,
        m_httpVersion,
        m_request.getMethod(),
        scheme,
        m_url.getHost()
    );

    if (m_url.getUsername().size() || m_url.getPassword().size()) {
        info.append("\nUsername: {}", m_url.getUsername());
        info.append("\nPassword: {}", m_url.getPassword());
    }

    info.append("\nPath: {}", m_url.getPath());

    if (!cutContent) {
        info.append("\nQuery: {}\nRequest Headers: {}\nBody:", m_url.stringifyQuery(), m_request.getHeaderList(false).contents);
        
        if (bodyContent.contents.size()) {
            info.append(bodyContent.type == ContentType::JSON ? ' ' : '\n');
            info.append(bodyContent.contents);
        }
    }

    if (m_url.getHash().size()) {
        info.append("\nHash: {}", m_url.getHash());
    }

    if (!this->hasResponse() || !this->m_response->m_received) return info.str();

    const Content responseContent = m_response->getResponseContent();

    info.append("\nStatus Code: {}\nResponse Time: {}ms", m_response->stringifyStatusCode(), m_response->getResponseTime());

    if (!cutContent) {
        info.append("\nResponse Headers: {}\nResponse:", m_response->getHeaderList(false).contents);

        if (responseContent.contents.size()) {
            info.append(responseContent.type == ContentType::JSON ? ' ' : '\n');
            info.append(responseContent.contents);
        }
    }
    

    return info.str();
}

std::string HttpInfo::toCurl() const {
    StringBuffer command;

    command.append("curl -X {} {}", m_request.getMethod(), m_url.getReconstruction());

    for (const auto& [key, values] : m_request.getHeaders()) {
        command.append(" -H \"{}: {}\"",
            StringStream::replace(key, "\"", "\\\""),
            StringStream::replace(StringStream::join(values, "; "), "\"", "\\\"")
        );
    }

    if (m_request.m_body.size()) {
        command.append(" -d \"{}\"", StringStream::replace(m_request.m_body, "\"", "\\\""));
    }

    return command.str();
}

void HttpInfo::resume() {
    m_state = State::IN_PROGRESS;
}

size_t HttpInfo::Request::getRequestTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::string HttpInfo::Request::stringifyMethod(const CCHttpRequest::HttpRequestType method) {
    switch (method) {
        case CCHttpRequest::HttpRequestType::kHttpGet: return "GET";
        case CCHttpRequest::HttpRequestType::kHttpPost: return "POST";
        case CCHttpRequest::HttpRequestType::kHttpPut: return "PUT";
        case CCHttpRequest::HttpRequestType::kHttpDelete: return "DELETE";
        default: return "UNKNOWN";
    }
}

HttpInfo::Request::Request(const URL& url, CCHttpRequest* request) : m_method(Request::stringifyMethod(request->getRequestType())),
m_path(url.getPath()),
m_headers(HttpInfo::parseCocosHeaders(request->getHeaders())),
m_body(std::string(request->getRequestData(), request->getRequestDataSize())),
m_contentType(HttpInfo::determineContentType(url.getPath(), true, m_body)),
m_startTime(Request::getRequestTime()) { }

HttpInfo::Request::Request(const URL& url, const WebRequest& request) : m_method(request.getMethod()),
m_path(url.getPath()),
m_headers(request.getHeaders()),
m_startTime(Request::getRequestTime()) {
    const ByteVector body = request.getBody().value_or(ByteVector());

    m_body = std::string(body.begin(), body.end());
    m_contentType = HttpInfo::determineContentType(url.getPath(), true, m_body);
}

HttpInfo::Content HttpInfo::Request::getHeaderList(const bool raw) const {
    return HttpInfo::getHeaders(raw, m_headers);
}

std::vector<std::string> HttpInfo::Request::getHeader(const std::string_view key) const {
    const Headers::const_iterator it = m_headers.find(key);

    if (it == m_headers.end()) {
        return {};
    } else {
        return it->second;
    }
}

HttpInfo::Content HttpInfo::Request::getBodyContent(const bool raw) const {
    return HttpInfo::getContent(raw, m_contentType, m_path, m_body);
}

void HttpInfo::Request::resetTime() {
    m_startTime = Request::getRequestTime();
}

size_t HttpInfo::Response::calculateResponseTime(std::shared_ptr<HttpInfo> info) {
    using namespace std::chrono;

    return duration_cast<milliseconds>(system_clock::now() - system_clock::time_point(milliseconds(info->m_request.getStartTime()))).count();
}

HttpInfo::Response::Response(std::shared_ptr<HttpInfo> info, const int code) : m_received(false),
m_responseTime(Response::calculateResponseTime(info)),
m_statusCode(code) { };

HttpInfo::Response::Response(std::shared_ptr<HttpInfo> info, CCHttpResponse* response) : m_received(true),
m_responseTime(Response::calculateResponseTime(info)),
m_path(info->m_url.getPath()),
m_headers(HttpInfo::parseCocosHeaders(response->getResponseHeader())),
m_statusCode(response->getResponseCode()) {
    gd::vector<char>* data = response->getResponseData();

    m_response = std::string(data->begin(), data->end());
    m_contentType = HttpInfo::determineContentType(info->m_url.getPath(), false, m_response);
}

HttpInfo::Response::Response(std::shared_ptr<HttpInfo> info, const WebResponse& response) : m_received(true),
m_responseTime(Response::calculateResponseTime(info)),
m_path(info->m_url.getPath()),
m_headers(Headers()),
m_statusCode(response.code()),
m_response(response.string().unwrapOrDefault()),
m_contentType(HttpInfo::determineContentType(info->m_url.getPath(), false, m_response)) {
    for (const std::string_view header : response.headers()) {
        m_headers[std::string(header)] = response.getAllHeadersNamed(header).value_or(std::vector<std::string>());
    }
}

std::string HttpInfo::Response::stringifyStatusCode() const {
    switch (m_statusCode) {
        case static_cast<int>(GeodeWebError::CURL_INITIALIZATION_ERROR): return "Curl Initialization Fail";
        case static_cast<int>(GeodeWebError::REQUEST_CANCELLED): return "Request Cancelled";
        case static_cast<int>(GeodeWebError::QUEUE_FULL): return "Async Queue Full";
        case static_cast<int>(GeodeWebError::CHANNEL_CLOSED): return "Channel Closed";
        default: return std::to_string(m_statusCode);
    }
}

HttpInfo::Content HttpInfo::Response::getHeaderList(const bool raw) const {
    return HttpInfo::getHeaders(raw, m_headers);
}

HttpInfo::Content HttpInfo::Response::getResponseContent(const bool raw) const {
    return HttpInfo::getContent(raw, m_contentType, m_path, m_response);
}

std::vector<std::string> HttpInfo::Response::getHeader(const std::string_view key) const {
    const Headers::const_iterator it = m_headers.find(key);

    if (it == m_headers.end()) {
        return {};
    } else {
        return it->second;
    }
}