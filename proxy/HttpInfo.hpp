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

        struct HttpContent {
            ContentType type;
            std::string contents;
        };

        std::string stringifyProtocol() const;
        std::string stringifyQuery() const;
        std::string stringifyHeaders() const;
        std::string stringifyStatusCode() const;
        HttpContent getBodyContent(const bool raw = true);
        HttpContent getResponseContent(const bool raw = true);
        bool isPaused() const;
        void resetCache();
    private:
        static converters::FormToJson formToJson;
        static converters::RobTopToJson robtopToJson;
        static converters::BinaryToRaw binaryToRaw;

        GETTER(Origin, origin, Origin)
        GETTER(Protocol, protocol, Protocol)
        GETTER(std::string, method, Method)
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
        HttpContent m_simplifiedBodyCache;
        HttpContent m_simplifiedResponseCache;
        bool m_paused;

        HttpInfo(CCHttpRequest* request);
        HttpInfo(web::WebRequest* request, const std::string& method, const std::string& url);
        HttpContent getContent(const bool raw, const ContentType originalContentType, const std::string& original, HttpContent& cache);
        HttpContent simplifyContent(const HttpContent& content);
        ContentType determineContentType(const std::string& content, const bool isBody = false);
        std::string determineMethod(CCHttpRequest::HttpRequestType method);
        bool isDomain(const std::string& domain);
        bool shouldPause();
        void resume();
        void determineOrigin();
        void parseUrl();

        friend struct ProxyHandler;
    };

    #undef GETTER
}