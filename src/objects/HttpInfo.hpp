#pragma once

#include "../include.hpp"
#include "converters/FormToJson.hpp"
#include "converters/RobTopToJson.hpp"
#include "converters/JsonConverter.hpp"
#include "converters/BinaryToRaw.hpp"

struct HttpInfo : public CCObject {
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

    static std::vector<HttpInfo*> requests;

    static HttpInfo* create(CCHttpRequest* request);

    HttpInfo(CCHttpRequest* request);
    bool isActive();
    void setActive(const bool active);
    std::string getHost();
    std::string getPath();
    ContentType getBodyContentType();
    ContentType getResponseContentType();
    std::string formatProtocol();
    std::string formatMethod();
    std::string formatQuery();
    std::string formatHeaders();
    std::pair<ContentType, std::string> formatBody();
    std::pair<ContentType, std::string> formatResponse();
    unsigned int getResponseCode();
    std::string generateBasicInfo(const bool withStatus = true);
    ccColor3B colorForMethod();
private:
    static FormToJson formToJson;
    static RobTopToJson robtopToJson;
    static BinaryToRaw binaryToRaw;

    bool m_active;
    Protocol m_protocol;
    CCHttpRequest::HttpRequestType m_method;
    std::string m_host;
    std::string m_path;
    std::string m_body;
    ContentType m_bodyContentType;
    std::string m_response;
    unsigned int m_responseCode;
    ContentType m_responseContentType;
    json m_query;
    json m_headers;
    CCObject* m_originalTarget;
    SEL_HttpResponse m_originalProxy;
    std::pair<ContentType, std::string> m_simplifiedBodyCache;
    std::pair<ContentType, std::string> m_simplifiedResponseCache;

    std::pair<ContentType, std::string> simplifyContent(const std::pair<HttpInfo::ContentType, std::string>& content);
    ContentType determineContentType(const std::string& content, const bool isBody = false);
    void onResponse(CCHttpClient* client, CCHttpResponse* response);
};