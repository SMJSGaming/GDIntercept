#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "../lib/json.hpp"
#include "Enums.hpp"
#include "../lib/LookupTable.hpp"
#include "converters/Converter.hpp"
#include "converters/XmlToXml.hpp"
#include "converters/FormToJson.hpp"
#include "converters/JsonToJson.hpp"
#include "converters/RawToBinary.hpp"
#include "converters/RobTopToJson.hpp"

namespace proxy {
    #define PROXY_GETTER(type, name, capital_name) \
        public: type get##capital_name() const { return m_##name; } \
        private: type m_##name

    class HttpInfo {
    public:
        struct HttpContent {
            enums::ContentType type;
            std::string contents;
        };

        class Request;

        class URL {
        public:
            std::string stringifyProtocol() const;
            std::string stringifyQuery(const bool raw = false) const;
            std::string getPortHost() const;
        private:
            static std::string stringifyMethod(const cocos2d::extension::CCHttpRequest::HttpRequestType method);
            static enums::Origin determineOrigin(const std::string& host);

            PROXY_GETTER(enums::Origin, origin, Origin);
            PROXY_GETTER(enums::Protocol, protocol, Protocol);
            PROXY_GETTER(std::string, method, Method);
            PROXY_GETTER(std::string, original, Original);
            PROXY_GETTER(std::string, queryLess, QueryLess);
            PROXY_GETTER(std::string, host, Host);
            PROXY_GETTER(int, port, Port);
            PROXY_GETTER(std::string, path, Path);
            PROXY_GETTER(nlohmann::json, query, Query);

            URL(cocos2d::extension::CCHttpRequest* request);
            URL(geode::utils::web::WebRequest* request, const std::string& method, const std::string& url);
            void parse();

            friend class HttpInfo::Request;
        };

        class Request {
        public:
            std::string stringifyHeaders(const bool raw = false) const;
            HttpContent getBodyContent(const bool raw = true) const;
        private:
            PROXY_GETTER(URL, url, URL);
            PROXY_GETTER(nlohmann::json, headers, Headers);
            PROXY_GETTER(std::string, body, Body);
            PROXY_GETTER(enums::ContentType, contentType, ContentType);

            Request(cocos2d::extension::CCHttpRequest* request);
            Request(geode::utils::web::WebRequest* request, const std::string& method, const std::string& url);

            friend class HttpInfo;
        };

        class Response {
        public:
            std::string stringifyHeaders(const bool raw = false) const;
            std::string stringifyStatusCode() const;
            HttpContent getResponseContent(const bool raw = true) const;
            bool received() const;
        private:
            PROXY_GETTER(nlohmann::json, headers, Headers);
            PROXY_GETTER(int, statusCode, StatusCode);
            PROXY_GETTER(std::string, response, Response);
            PROXY_GETTER(enums::ContentType, contentType, ContentType);
            Request* m_request;
            bool m_received;

            Response();
            Response(Request* request, cocos2d::extension::CCHttpResponse* response);
            Response(Request* request, geode::utils::web::WebResponse* response);

            friend class ProxyHandler;
            friend class HttpInfo;
        };

        void cancel();
        bool isPaused() const;
        bool isInProgress() const;
        bool isCompleted() const;
        bool isFaulty() const;
        bool isCancelled() const;
        bool isRepeat() const;
        bool responseReceived() const;
    private:
        static const LookupTable<enums::ContentType, converters::Converter*> converters;

        static HttpContent getContent(const bool raw, const enums::ContentType originalContentType, const std::string& path, const std::string& original);
        static enums::ContentType determineContentType(const std::string& path, const bool isBody, const std::string& content);
        static nlohmann::json parseCocosHeaders(const gd::vector<char>* headers);
        static nlohmann::json parseCocosHeaders(const gd::vector<gd::string>& headers);

        PROXY_GETTER(size_t, id, ID);
        PROXY_GETTER(enums::Client, client, Client);
        PROXY_GETTER(enums::State, state, State);
        PROXY_GETTER(Request, request, Request);
        PROXY_GETTER(Response, response, Response);
        bool m_repeat;

        HttpInfo(const bool paused, cocos2d::extension::CCHttpRequest* request);
        HttpInfo(const bool paused, const bool repeat, geode::utils::web::WebRequest* request, const std::string& method, const std::string& url);
        void resume();

        friend class ProxyHandler;
    };

    #undef PROXY_GETTER
}