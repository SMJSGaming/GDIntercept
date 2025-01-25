#include "../../proxy/Proxy.hpp"
#include "../../proxy/HttpInfo.hpp"

using namespace nlohmann;
using namespace geode::prelude;
using namespace proxy::prelude;

static size_t globalIndexCounter = 1;

const LookupTable<ContentType, proxy::converters::Converter*> proxy::HttpInfo::converters({
    { ContentType::XML, new XmlToXml() },
    { ContentType::JSON, new JsonToJson() },
    { ContentType::ROBTOP, new RobTopToJson() },
    { ContentType::FORM, new FormToJson() },
    { ContentType::BINARY, new RawToBinary() }
});

proxy::HttpInfo::Content proxy::HttpInfo::getContent(const bool raw, const ContentType originalContentType, const std::string& path, const std::string& original) {
    if (HttpInfo::converters.contains(originalContentType)) {
        const converters::Converter* converter = HttpInfo::converters.at(originalContentType);

        if (!raw) {
            return { converter->resultContentType(), converter->convert(path, original) };
        } else if (converter->needsSanitization()) {
            return { originalContentType, converter->toRaw(path, converter->convert(path, original)) };
        }
    }

    return { originalContentType, original };
}

proxy::HttpInfo::Content proxy::HttpInfo::getHeaders(const bool raw, const Headers& headers) {
    if (raw) {
        std::stringstream stream;

        for (const auto& [headerKey, list] : headers) {
            for (const std::string& headerValue : list) {
                stream << headerKey << ':' << headerValue << std::endl;
            }
        }

        return { ContentType::UNKNOWN_CONTENT, stream.str() };
    } else {
        json content = json::object();

        for (const auto& [headerKey, list] : headers) {
            content[headerKey] = list.size() == 1 ? json(list.at(0)) : json(list);
        }

        return { ContentType::JSON, converters::safeDump(content) };
    }
}

ContentType proxy::HttpInfo::determineContentType(const std::string& path, const bool isBody, const std::string& content) {
    if (content.empty()) {
        return ContentType::UNKNOWN_CONTENT;
    }

    for (const auto& [type, converter] : HttpInfo::converters) {
        if (converter->canConvert(path, isBody, content)) {
            return type;
        }
    }

    return ContentType::UNKNOWN_CONTENT;
}

proxy::HttpInfo::Headers proxy::HttpInfo::parseCocosHeaders(const gd::vector<char>* headers) {
    return HttpInfo::parseCocosHeaders(StringStreamer::of(std::string(headers->begin(), headers->end())).map<gd::string>([](std::string header) {
        if (header.back() == '\r') {
            header.pop_back();
        }

        return header;
    }).filter([](const std::string& header) {
        return header.size();
    }));
}

proxy::HttpInfo::Headers proxy::HttpInfo::parseCocosHeaders(const gd::vector<gd::string>& headers) {
    Headers parsed;

    // CCHttp headers technically allow for weird header formats, but I'm assuming they're all key-value pairs separated by a colon since this is the standard
    // If you don't follow the standard, I'm not going to care that you get shitty results
    for (const gd::string& header : headers) {
        const std::string headerString(header.c_str());
        const size_t colonPos = headerString.find(":");
        std::string headerKey = headerString;
        std::string headerValue;

        if (colonPos != std::string::npos) {
            std::string value(headerString.substr(colonPos + 1));

            headerKey = headerString.substr(0, colonPos);
            headerValue = value.erase(0, value.find_first_not_of(' '));
        }

        if (parsed.contains(headerKey)) {
            parsed[headerKey].push_back(headerValue);
        } else {
            parsed.insert({ headerKey, { headerValue } });
        }
    }

    return parsed;
}

proxy::HttpInfo::HttpInfo(const bool paused, CCHttpRequest* request) : m_id(globalIndexCounter++),
m_client(Client::COCOS),
m_state(paused ? State::PAUSED : State::IN_PROGRESS),
m_repeat(false),
m_request(request) { }

proxy::HttpInfo::HttpInfo(const bool paused, const bool repeat, web::WebRequest* request, const std::string& method, const std::string& url) : m_id(globalIndexCounter++),
m_client(Client::GEODE),
m_state(paused ? State::PAUSED : State::IN_PROGRESS),
m_repeat(repeat),
m_request(request, method, url) { }

void proxy::HttpInfo::cancel() {
    if (m_state == State::IN_PROGRESS || m_state == State::PAUSED) {
        m_state = State::CANCELLED;
        m_response.m_statusCode = -3;

        CancelEvent(this).post();
    }
}

bool proxy::HttpInfo::isPaused() const {
    return m_state == State::PAUSED;
}

bool proxy::HttpInfo::isInProgress() const {
    return m_state == State::IN_PROGRESS;
}

bool proxy::HttpInfo::isCompleted() const {
    return m_state == State::COMPLETED;
}

bool proxy::HttpInfo::isFaulty() const {
    return m_state == State::FAULTY;
}

bool proxy::HttpInfo::isCancelled() const {
    return m_state == State::CANCELLED;
}

bool proxy::HttpInfo::isRepeat() const {
    return m_repeat;
}

bool proxy::HttpInfo::responseReceived() const {
    return m_state == State::COMPLETED || m_state == State::FAULTY;
}

void proxy::HttpInfo::resume() {
    m_state = State::IN_PROGRESS;
}

std::string proxy::HttpInfo::Request::stringifyMethod(const CCHttpRequest::HttpRequestType method) {
    switch (method) {
        case CCHttpRequest::HttpRequestType::kHttpGet: return "GET";
        case CCHttpRequest::HttpRequestType::kHttpPost: return "POST";
        case CCHttpRequest::HttpRequestType::kHttpPut: return "PUT";
        case CCHttpRequest::HttpRequestType::kHttpDelete: return "DELETE";
        default: return "UNKNOWN";
    }
}

proxy::HttpInfo::Request::Request(CCHttpRequest* request) : m_method(Request::stringifyMethod(request->getRequestType())),
m_url(request->getUrl()),
m_headers(HttpInfo::parseCocosHeaders(request->getHeaders())),
m_body(std::string(request->getRequestData(), request->getRequestDataSize())),
m_contentType(HttpInfo::determineContentType(m_url.getPath(), true, m_body)) { }

proxy::HttpInfo::Request::Request(web::WebRequest* request, const std::string& method, const std::string& url) : m_method(method),
m_url(url, request),
m_headers(request->getHeaders()) {
    const ByteVector body = request->getBody().value_or(ByteVector());

    m_body = std::string(body.begin(), body.end());
    m_contentType = HttpInfo::determineContentType(m_url.getPath(), true, m_body);
}

proxy::HttpInfo::Content proxy::HttpInfo::Request::getHeaderList(const bool raw) const {
    return HttpInfo::getHeaders(raw, m_headers);
}

proxy::HttpInfo::Content proxy::HttpInfo::Request::getBodyContent(const bool raw) const {
    return HttpInfo::getContent(raw, m_contentType, m_url.getPath(), m_body);
}

proxy::HttpInfo::Response::Response() : m_statusCode(0), m_received(false) { };

proxy::HttpInfo::Response::Response(Request* request, CCHttpResponse* response, const size_t responseTime) : m_received(true),
m_responseTime(responseTime),
m_request(request),
m_headers(HttpInfo::parseCocosHeaders(response->getResponseHeader())),
m_statusCode(response->getResponseCode()) {
    gd::vector<char>* data = response->getResponseData();

    m_response = std::string(data->begin(), data->end());
    m_contentType = HttpInfo::determineContentType(request->m_url.getPath(), false, m_response);
}

proxy::HttpInfo::Response::Response(Request* request, web::WebResponse* response, const size_t responseTime) : m_received(true),
m_responseTime(responseTime),
m_request(request),
m_headers(Headers()),
m_statusCode(response->code()),
m_response(response->string().unwrapOrDefault()),
m_contentType(HttpInfo::determineContentType(request->m_url.getPath(), false, m_response)) {
    for (const std::string& header : response->headers()) {
        m_headers[header] = response->getAllHeadersNamed(header).value_or(std::vector<std::string>());
    }
}

std::string proxy::HttpInfo::Response::stringifyStatusCode() const {
    switch (m_statusCode) {
        case -3: return "Request Cancelled";
        case -2: return "Request Timeout";
        case -1: return "Request Error";
        case 0: return "No response available yet";
        default: return std::to_string(m_statusCode);
    }
}

proxy::HttpInfo::Content proxy::HttpInfo::Response::getHeaderList(const bool raw) const {
    return HttpInfo::getHeaders(raw, m_headers);
}

proxy::HttpInfo::Content proxy::HttpInfo::Response::getResponseContent(const bool raw) const {
    return HttpInfo::getContent(raw, m_contentType, m_request->getURL().getPath(), m_response);
}

bool proxy::HttpInfo::Response::received() const {
    return m_received;
}