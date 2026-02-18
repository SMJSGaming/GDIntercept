#pragma once

#include "BaseProxy.hpp"
#include "converters/Converter.hpp"
#include "converters/FormToJson.hpp"

namespace proxy {
    class URL {
    public:
        std::string getBasicUrl() const;
        std::string stringifyQuery(const bool raw = false) const;

        URL(std::string url);
        URL(std::string url, const geode::utils::web::WebRequest& request);
        URL(const URL&) = delete;
        URL& operator=(const URL&) = delete;
    private:
        PROXY_PRIMITIVE_GETTER(enums::Origin, origin, Origin);
        PROXY_PRIMITIVE_GETTER(enums::Scheme, scheme, Scheme);
        PROXY_GETTER(std::string, original, Original);
        PROXY_GETTER(std::string, reconstruction, Reconstruction);
        PROXY_GETTER(std::string, stringScheme, StringScheme);
        PROXY_GETTER(std::string, username, Username);
        PROXY_GETTER(std::string, password, Password);
        PROXY_GETTER(std::string, domainName, DomainName);
        PROXY_GETTER(std::string, domain, Domain);
        PROXY_GETTER(std::vector<std::string>, subDomains, SubDomains);
        PROXY_GETTER(std::string, host, Host);
        PROXY_GETTER(std::string, tld, TLD);
        PROXY_PRIMITIVE_GETTER(int, port, Port);
        PROXY_GETTER(std::string, path, Path);
        PROXY_GETTER(std::string, queryString, QueryString);
        PROXY_GETTER(std::string, hash, Hash);
        PROXY_GETTER(nlohmann::ordered_json, query, Query);

        void parse();
        void parseScheme(size_t& index);
        void parseLogin(size_t& index);
        void parseHost(size_t& index);
        void parsePath(size_t& index);
        void determineOrigin();
    };
}