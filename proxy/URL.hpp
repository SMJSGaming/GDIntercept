#pragma once

#include "BaseProxy.hpp"
#include "converters/Converter.hpp"
#include "converters/FormToJson.hpp"

namespace proxy {
    class URL {
    public:
        static void load();

        std::string getBasicUrl() const;
        std::string stringifyQuery(const bool raw = false) const;

        URL(const std::string& url, geode::utils::web::WebRequest* request = nullptr);
    private:
        static std::vector<std::string> suffixes;

        PROXY_GETTER(enums::Origin, origin, Origin);
        PROXY_GETTER(std::string, original, Original);
        PROXY_GETTER(enums::Protocol, scheme, Scheme);
        PROXY_GETTER(std::string, protocol, Protocol);
        PROXY_GETTER(std::string, username, Username);
        PROXY_GETTER(std::string, password, Password);
        PROXY_GETTER(std::string, domainName, DomainName);
        PROXY_GETTER(std::string, domain, Domain);
        PROXY_GETTER(std::vector<std::string>, subDomains, SubDomains);
        PROXY_GETTER(std::vector<std::string>, sld, SLD);
        PROXY_GETTER(std::string, host, Host);
        PROXY_GETTER(std::string, tld, TLD);
        PROXY_GETTER(int, port, Port);
        PROXY_GETTER(std::string, path, Path);
        PROXY_GETTER(std::string, queryString, QueryString);
        PROXY_GETTER(std::string, hash, Hash);
        PROXY_GETTER(nlohmann::json, query, Query);

        void parse();
        void parseProtocol(size_t& index);
        void parseLogin(size_t& index);
        void parseHost(size_t& index);
        void parsePath(size_t& index);
        void determineOrigin();
    };
}