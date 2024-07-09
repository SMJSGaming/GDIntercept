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

    class HttpInfo {
    public:
        enum ContentType {
            FORM,
            JSON,
            XML,
            ROBTOP,
            BINARY,
            UNKNOWN_CONTENT
        };

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

        struct HttpContent {
            ContentType type;
            std::string contents;
        };

        class Request;

        class URL {
        public:
            std::string stringifyProtocol() const;
            std::string stringifyQuery() const;
        private:
            static std::string stringifyMethod(const CCHttpRequest::HttpRequestType method);
            static Origin determineOrigin(const std::string& host);

            GETTER(Origin, origin, Origin)
            GETTER(Protocol, protocol, Protocol)
            GETTER(std::string, method, Method)
            GETTER(std::string, raw, Raw)
            GETTER(std::string, host, Host)
            GETTER(std::string, path, Path)
            GETTER(json, query, Query)

            URL(CCHttpRequest* request);
            URL(web::WebRequest* request, const std::string& method, const std::string& url);
            void parse();

            friend class HttpInfo::Request;
        };

        class Request {
        public:
            std::string stringifyHeaders() const;
            HttpContent getBodyContent() const;
            HttpContent getBodyContent(const bool raw);
            void resetCache();
        private:
            GETTER(URL, url, URL)
            GETTER(json, headers, Headers)
            GETTER(std::string, body, Body)
            GETTER(ContentType, contentType, ContentType)
            HttpContent m_simplifiedBodyCache;

            Request(CCHttpRequest* request);
            Request(web::WebRequest* request, const std::string& method, const std::string& url);

            friend class HttpInfo;
        };

        class Response {
        public:
            std::string stringifyHeaders() const;
            std::string stringifyStatusCode() const;
            HttpContent getResponseContent() const;
            HttpContent getResponseContent(const bool raw = true);
            bool received() const;
            void resetCache();
        private:
            GETTER(json, headers, Headers)
            GETTER(int, statusCode, StatusCode)
            GETTER(std::string, response, Response)
            GETTER(ContentType, contentType, ContentType)
            HttpContent m_simplifiedResponseCache;
            Request* m_request;
            bool m_received;

            Response();
            Response(Request* request, CCHttpResponse* response);
            Response(Request* request, web::WebResponse* response);

            friend class ProxyHandler;
            friend class HttpInfo;
        };

        static converters::FormToJson formToJson;
        static converters::RobTopToJson robtopToJson;
        static converters::BinaryToRaw binaryToRaw;

        bool isPaused() const;
        bool responseReceived() const;
        void resetCache();
    private:
        static HttpContent getContent(const bool raw, const ContentType originalContentType, const std::string& path, const std::string& original, HttpContent& cache);
        static HttpContent simplifyContent(const std::string& path, const HttpContent& content);
        static ContentType determineContentType(const std::string& path, const std::string& content, const bool isBody = false);
        static json parseCocosHeaders(const std::vector<char>* headers);
        static json parseCocosHeaders(const std::vector<gd::string>& headers);
        static bool shouldPause();

        GETTER(Request, request, Request)
        GETTER(Response, response, Response)
        bool m_paused;

        HttpInfo(CCHttpRequest* request);
        HttpInfo(web::WebRequest* request, const std::string& method, const std::string& url);
        void resume();

        friend class ProxyHandler;
    };

    #undef GETTER
}