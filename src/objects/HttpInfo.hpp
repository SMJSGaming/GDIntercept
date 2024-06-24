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

    typedef std::pair<ContentType, std::string> content;

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
    content formatBody();
    content formatResponse();
    unsigned int getResponseCode();
    std::string generateBasicInfo(const bool withStatus = true);
    ccColor3B colorForMethod();
    void resetCache();
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
    content m_simplifiedBodyCache;
    content m_simplifiedResponseCache;

    content getContent(const ContentType originalContentType, const std::string& original, content& cache);
    content simplifyContent(const content& content);
    ContentType determineContentType(const std::string& content, const bool isBody = false);
    void onResponse(CCHttpClient* client, CCHttpResponse* response);
};