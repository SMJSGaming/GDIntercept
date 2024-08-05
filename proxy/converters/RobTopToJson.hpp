#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class RobTopToJson : public Converter {
    public:
        enums::ContentType resultContentType() const override;
        bool needsSanitization() const override;
        bool canConvert(const std::string& path, const bool isBody, const std::string& original) const override;
        std::string convert(const std::string& path, const std::string& original) const override;
        std::string toRaw(const std::string& path, const std::string& original) const override;
    private:
        enum ObjectType {
            OBJECT,
            ARRAY
        };

        // The only reason why const char* is involved here is because std::string basically sees everything as a string ctor so overloading with it was almost impossible
        class ObjParser {
        public:
            ObjParser(const char* delimiter, const char* entryDelimiter = "");
            ObjParser(const std::vector<std::string>& tupleKeys, const char* delimiter = "", const char* entryDelimiter = "");
            virtual nlohmann::json parse(const std::string& str) const;
            virtual std::string toRaw(const nlohmann::json& json) const;
        private:
            ObjectType m_bodyType;
            std::string m_delimiter;
            std::string m_entryDelimiter;
            std::vector<std::string> m_tupleKeys;

            static std::vector<std::string> split(const std::string& str, const std::string& delimiter);

            nlohmann::json parseEntry(const std::string& str) const;
            std::string toRawEntry(const nlohmann::json& json) const;
        };

        class Parser : public ObjParser {
        public:
            Parser(const char* delimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
            Parser(const std::vector<std::string>& tupleKeys, const char* delimiter = "", const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
            Parser(const char* delimiter, const char* entryDelimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
            Parser(const std::vector<std::string>& tupleKeys, const char* delimiter, const char* entryDelimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
            nlohmann::json parse(const std::string& str) const override;
            std::string toRaw(const nlohmann::json& json) const override;
        private:
            std::vector<std::tuple<std::string, ObjParser>> m_metadata;
        };

        static const std::unordered_map<std::string, Parser> parsers;
    };
}