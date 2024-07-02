#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "../lib/json.hpp"
#include "converters/Converter.hpp"
#include "converters/FormToJson.hpp"
#include "converters/BinaryToRaw.hpp"
#include "converters/RobTopToJson.hpp"

namespace proxy {
    #define GETTER(type, name, capital_name) \
        public: \
            type get##capital_name() const { return m_##name; } \
        private: \
            type m_##name;

    using namespace geode::prelude;
    using namespace nlohmann;

    struct HttpInfo {
        enum Origin {
            GD,
            GD_CDN,
            ROBTOP_GAMES,
            NEWGROUNDS,
            GEODE,
            OTHER
        };

        enum Protocol {
            HTTP,
            HTTPS,
            UNKNOWN_PROTOCOL
        };

        enum ContentType {
            FORM,
            JSON,
            XML,
            ROBTOP,
            BINARY,
            UNKNOWN_CONTENT
        };

        typedef std::pair<ContentType, std::string> content;

        std::string stringifyProtocol() const;
        std::string stringifyMethod() const;
        std::string stringifyQuery() const;
        std::string stringifyHeaders() const;
        content getBodyContent(const bool raw = true);
        content getResponseContent(const bool raw = true);
        void resetCache();
    private:
        static converters::FormToJson formToJson;
        static converters::RobTopToJson robtopToJson;
        static converters::BinaryToRaw binaryToRaw;

        GETTER(Origin, origin, Origin)
        GETTER(Protocol, protocol, Protocol)
        GETTER(CCHttpRequest::HttpRequestType, method, Method)
        GETTER(std::string, url, Url)
        GETTER(std::string, host, Host)
        GETTER(std::string, path, Path)
        GETTER(json, query, Query)
        GETTER(json, headers, Headers)
        GETTER(std::string, body, Body)
        GETTER(ContentType, bodyContentType, BodyContentType)
        GETTER(int, statusCode, StatusCode)
        GETTER(std::string, response, Response)
        GETTER(ContentType, responseContentType, ResponseContentType)
        content m_simplifiedBodyCache;
        content m_simplifiedResponseCache;

        HttpInfo(CCHttpRequest* request);
        HttpInfo(web::WebRequest* request, const std::string& method, const std::string& url);
        content getContent(const bool raw, const ContentType originalContentType, const std::string& original, content& cache);
        content simplifyContent(const content& content);
        ContentType determineContentType(const std::string& content, const bool isBody = false);
        bool isDomain(const std::string& domain);
        void determineOrigin();
        void parseUrl(const std::string& url);

        friend struct ProxyHandler;
    };

    #undef GETTER
}