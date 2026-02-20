#pragma once

#include "../include.hpp"
#include "Enums.hpp"
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
        PRIMITIVE_GETTER(enums::Origin, origin, Origin);
        PRIMITIVE_GETTER(enums::Scheme, scheme, Scheme);
        GETTER(std::string, original, Original);
        GETTER(std::string, reconstruction, Reconstruction);
        GETTER(std::string, stringScheme, StringScheme);
        GETTER(std::string, username, Username);
        GETTER(std::string, password, Password);
        GETTER(std::string, domainName, DomainName);
        GETTER(std::string, domain, Domain);
        GETTER(std::vector<std::string>, subDomains, SubDomains);
        GETTER(std::string, host, Host);
        GETTER(std::string, tld, TLD);
        PRIMITIVE_GETTER(int, port, Port);
        GETTER(std::string, path, Path);
        GETTER(std::string, queryString, QueryString);
        GETTER(std::string, hash, Hash);
        GETTER(nlohmann::ordered_json, query, Query);

        void parse();
        void parseScheme(size_t& index);
        void parseLogin(size_t& index);
        void parseHost(size_t& index);
        void parsePath(size_t& index);
        void determineOrigin();
    };
}