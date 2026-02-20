#pragma once

#include "../include.hpp"
#include "converters/Converter.hpp"
#include "converters/XmlToXml.hpp"
#include "converters/FormToJson.hpp"
#include "converters/JsonToJson.hpp"
#include "converters/RawToBinary.hpp"
#include "converters/RobTopToJsonV2.hpp"
#include "URL.hpp"

namespace proxy {
    class HttpInfo : std::enable_shared_from_this<HttpInfo> {
    public:
        struct Content {
            enums::ContentType type;
            std::string contents;
        };

        typedef geode::utils::StringMap<std::vector<std::string>> Headers;

        class Request {
        public:
            Content getHeaderList(const bool raw = false) const;
            Content getBodyContent(const bool raw = true) const;
            std::vector<std::string> getHeader(const std::string_view key) const;
        private:
            static size_t getRequestTime();
            static std::string stringifyMethod(const cocos2d::extension::CCHttpRequest::HttpRequestType method);

            GETTER(std::string, method, Method);
            GETTER(Headers, headers, Headers);
            GETTER(std::string, body, Body);
            PRIMITIVE_GETTER(size_t, startTime, StartTime);
            PRIMITIVE_GETTER(enums::ContentType, contentType, ContentType);
            std::string_view m_path;

            Request(const URL& url, cocos2d::extension::CCHttpRequest* request);
            Request(const URL& url, const geode::utils::web::WebRequest& request);
            Request(const Request&) = delete;
            Request& operator=(const Request&) = delete;
            void resetTime();

            friend class ProxyHandler;
            friend class HttpInfo;
        };

        class Response {
        public:
            std::string stringifyStatusCode() const;
            Content getHeaderList(const bool raw = false) const;
            Content getResponseContent(const bool raw = true) const;
            std::vector<std::string> getHeader(const std::string_view key) const;
        private:
            static size_t calculateResponseTime(std::shared_ptr<HttpInfo> info);

            GETTER(Headers, headers, Headers);
            PRIMITIVE_GETTER(int, statusCode, StatusCode);
            PRIMITIVE_GETTER(size_t, responseTime, ResponseTime);
            GETTER(std::string, response, Response);
            PRIMITIVE_GETTER(enums::ContentType, contentType, ContentType);
            PRIMITIVE_GETTER(bool, received, Received);
            std::string_view m_path;

            Response(std::shared_ptr<HttpInfo> info, const int code);
            Response(std::shared_ptr<HttpInfo> info, cocos2d::extension::CCHttpResponse* response);
            Response(std::shared_ptr<HttpInfo> info, const geode::utils::web::WebResponse& response);
            Response& operator=(const Response&) = delete;

            friend class ProxyHandler;
            friend class HttpInfo;
        };

        HttpInfo(cocos2d::extension::CCHttpRequest* request);
        HttpInfo(const bool repeat, const geode::utils::web::WebRequest& request);
        HttpInfo(const HttpInfo&) = delete;
        HttpInfo& operator=(const HttpInfo&) = delete;

        void cancel();
        bool isPaused() const;
        bool isInProgress() const;
        bool isCompleted() const;
        bool isFaulty() const;
        bool isCancelled() const;
        bool hasResponse() const;
        std::string toString(const bool cutContent = false) const;
        std::string toCurl() const;
    private:
        static const LookupTable<enums::ContentType, converters::Converter*> CONVERTERS;

        static Content getContent(const bool raw, const enums::ContentType originalContentType, const std::string_view path, const std::string_view original);
        static Content getHeaders(const bool raw, const Headers& headers);
        static enums::ContentType determineContentType(const std::string_view path, const bool isBody, const std::string_view content);
        static std::string translateHttpVersion(const geode::utils::web::HttpVersion version);
        static Headers parseCocosHeaders(const gd::vector<char>* headers);
        static Headers parseCocosHeaders(const gd::vector<gd::string>& headers);

        PRIMITIVE_GETTER(size_t, id, ID);
        PRIMITIVE_GETTER(enums::Client, client, Client);
        PRIMITIVE_GETTER(enums::State, state, State);
        GETTER(URL, url, URL);
        GETTER(Request, request, Request);
        GETTER(std::optional<Response>, response, Response);
        GETTER(std::string, httpVersion, HttpVersion);
        PRIMITIVE_GETTER(bool, repeat, Repeat);

        void resume();

        friend class ProxyHandler;
    };
}