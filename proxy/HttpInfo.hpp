#pragma once

#include "BaseProxy.hpp"
#include "../lib/Stream.hpp"
#include "../lib/LookupTable.hpp"
#include "converters/Converter.hpp"
#include "converters/XmlToXml.hpp"
#include "converters/FormToJson.hpp"
#include "converters/JsonToJson.hpp"
#include "converters/RawToBinary.hpp"
#include "converters/RobTopToJsonV2.hpp"
#include "URL.hpp"

namespace proxy {
    class HttpInfo {
    public:
        struct Content {
            enums::ContentType type;
            std::string contents;
        };

        typedef std::unordered_map<std::string, std::vector<std::string>> Headers;

        class Request {
        public:
            Content getHeaderList(const bool raw = false) const;
            Content getBodyContent(const bool raw = true) const;
        private:
            static std::string stringifyMethod(const cocos2d::extension::CCHttpRequest::HttpRequestType method);

            PROXY_GETTER(std::string, method, Method);
            PROXY_GETTER(URL, url, URL);
            PROXY_GETTER(Headers, headers, Headers);
            PROXY_GETTER(std::string, body, Body);
            PROXY_PRIMITIVE_GETTER(enums::ContentType, contentType, ContentType);

            Request(cocos2d::extension::CCHttpRequest* request);
            Request(geode::utils::web::WebRequest* request, std::string method, std::string url);

            friend class HttpInfo;
        };

        class Response {
        public:
            std::string stringifyStatusCode() const;
            Content getHeaderList(const bool raw = false) const;
            Content getResponseContent(const bool raw = true) const;
        private:
            PROXY_GETTER(Headers, headers, Headers);
            PROXY_PRIMITIVE_GETTER(int, statusCode, StatusCode);
            PROXY_PRIMITIVE_GETTER(size_t, responseTime, ResponseTime);
            PROXY_GETTER(std::string, response, Response);
            PROXY_PRIMITIVE_GETTER(enums::ContentType, contentType, ContentType);
            PROXY_PRIMITIVE_GETTER(bool, received, Received);
            Request* m_request;

            Response();
            Response(Request* request, cocos2d::extension::CCHttpResponse* response, const size_t responseTime);
            Response(Request* request, geode::utils::web::WebResponse* response, const size_t responseTime);

            friend class ProxyHandler;
            friend class HttpInfo;
        };

        void cancel();
        bool isPaused() const;
        bool isInProgress() const;
        bool isCompleted() const;
        bool isFaulty() const;
        bool isCancelled() const;
        bool responseReceived() const;
    private:
        static const LookupTable<enums::ContentType, converters::Converter*> CONVERTERS;

        static Content getContent(const bool raw, const enums::ContentType originalContentType, const std::string_view path, const std::string_view original);
        static Content getHeaders(const bool raw, const Headers& headers);
        static enums::ContentType determineContentType(const std::string_view path, const bool isBody, const std::string_view content);
        static Headers parseCocosHeaders(const gd::vector<char>* headers);
        static Headers parseCocosHeaders(const gd::vector<gd::string>& headers);

        PROXY_PRIMITIVE_GETTER(size_t, id, ID);
        PROXY_PRIMITIVE_GETTER(enums::Client, client, Client);
        PROXY_PRIMITIVE_GETTER(enums::State, state, State);
        PROXY_GETTER(Request, request, Request);
        PROXY_GETTER(Response, response, Response);
        PROXY_PRIMITIVE_GETTER(bool, repeat, Repeat);

        HttpInfo(const bool paused, cocos2d::extension::CCHttpRequest* request);
        HttpInfo(const bool paused, const bool repeat, geode::utils::web::WebRequest* request, std::string method, std::string url);
        void resume();

        friend class ProxyHandler;
    };

    #undef PROXY_GETTER
    #undef PROXY_PRIMITIVE_GETTER
}