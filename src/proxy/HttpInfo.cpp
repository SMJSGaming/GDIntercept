#include "../../proxy/Proxy.hpp"
#include "../../proxy/HttpInfo.hpp"

using namespace nlohmann;
using namespace geode::prelude;
using namespace proxy::prelude;

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

HttpInfo::Headers HttpInfo::parseCocosHeaders(const gd::vector<char>* headers) {
    return HttpInfo::parseCocosHeaders(StringStream::of(std::string_view(headers->begin(), headers->end()), '\n')
        .map<gd::string>([](std::string&& header) {
            if (header.back() == '\r') {
                header.pop_back();
            }

            return header;
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
m_request(request) { }

HttpInfo::HttpInfo(const bool repeat, const geode::utils::web::WebRequest& request) : m_id(globalIndexCounter++),
m_client(Client::GEODE),
m_state(State::IN_PROGRESS),
m_repeat(repeat),
m_request(request) { }

void HttpInfo::cancel() {
    if (m_state == State::IN_PROGRESS || m_state == State::PAUSED) {
        m_state = State::CANCELLED;
        m_response.emplace(Response(static_cast<int>(web::GeodeWebError::REQUEST_CANCELLED)));

        ProxyEvent(EventState::CANCEL).send(shared_from_this());
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

std::string HttpInfo::toString() const {
    const URL& url = m_request.getURL();
    Content bodyContent = m_request.getBodyContent();
    utils::StringBuffer out;

    out.append("Method: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nRequest Headers: {}\nBody:",
        m_request.getMethod(),
        url.getProtocol(),
        url.getHost(),
        url.getPath(),
        url.stringifyQuery(),
        m_request.getHeaderList(false).contents
    );

    if (!bodyContent.contents.empty()) {
        out.append(bodyContent.type == ContentType::JSON ? ' ' : '\n');
        out.append(bodyContent.contents);
    }

    if (this->hasResponse()) {
        const Content responseContent = m_response->getResponseContent();

        out.append("\nResponse Headers: {}\nResponse:", m_response->getHeaderList(false).contents);
        out.append(responseContent.type == ContentType::JSON ? ' ' : '\n');
        out.append(responseContent.contents);
    }

    return out.str();
}

void HttpInfo::resume() {
    m_state = State::IN_PROGRESS;
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

HttpInfo::Request::Request(CCHttpRequest* request) : m_method(Request::stringifyMethod(request->getRequestType())),
m_url(request->getUrl()),
m_headers(HttpInfo::parseCocosHeaders(request->getHeaders())),
m_body(std::string(request->getRequestData(), request->getRequestDataSize())),
m_contentType(HttpInfo::determineContentType(m_url.getPath(), true, m_body)) { }

HttpInfo::Request::Request(const web::WebRequest& request) : m_method(request.getMethod()),
m_url(request.getUrl(), request),
m_headers(request.getHeaders()) {
    const ByteVector body = request.getBody().value_or(ByteVector());

    m_body = std::string(body.begin(), body.end());
    m_contentType = HttpInfo::determineContentType(m_url.getPath(), true, m_body);
}

HttpInfo::Content HttpInfo::Request::getHeaderList(const bool raw) const {
    return HttpInfo::getHeaders(raw, m_headers);
}

HttpInfo::Content HttpInfo::Request::getBodyContent(const bool raw) const {
    return HttpInfo::getContent(raw, m_contentType, m_url.getPath(), m_body);
}

HttpInfo::Response::Response(const int code) : m_statusCode(code), m_received(false) { };

HttpInfo::Response::Response(const Request* request, CCHttpResponse* response, const size_t responseTime) : m_received(true),
m_responseTime(responseTime),
m_request(request),
m_headers(HttpInfo::parseCocosHeaders(response->getResponseHeader())),
m_statusCode(response->getResponseCode()) {
    gd::vector<char>* data = response->getResponseData();

    m_response = std::string(data->begin(), data->end());
    m_contentType = HttpInfo::determineContentType(request->m_url.getPath(), false, m_response);
}

HttpInfo::Response::Response(const Request* request, const web::WebResponse& response, const size_t responseTime) : m_received(true),
m_responseTime(responseTime),
m_request(request),
m_headers(Headers()),
m_statusCode(response.code()),
m_response(response.string().unwrapOrDefault()),
m_contentType(HttpInfo::determineContentType(request->m_url.getPath(), false, m_response)) {
    for (const std::string_view header : response.headers()) {
        m_headers[std::string(header)] = response.getAllHeadersNamed(header).value_or(std::vector<std::string>());
    }
}

std::string HttpInfo::Response::stringifyStatusCode() const {
    switch (m_statusCode) {
        case -3: return "Request Cancelled";
        case -2: return "Request Timeout";
        case -1: return "Request Error";
        case 0: return "No response available yet";
        default: return std::to_string(m_statusCode);
    }
}

HttpInfo::Content HttpInfo::Response::getHeaderList(const bool raw) const {
    return HttpInfo::getHeaders(raw, m_headers);
}

HttpInfo::Content HttpInfo::Response::getResponseContent(const bool raw) const {
    return HttpInfo::getContent(raw, m_contentType, m_request->getURL().getPath(), m_response);
}