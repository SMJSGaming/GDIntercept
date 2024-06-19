#pragma once

#include "JsonConverter.hpp"

struct RobTopToJson : public JsonConverter {
    enum ObjectType {
        OBJECT,
        ARRAY
    };

    enum SeparatorType {
        KEY_VALUE,
        TUPLE
    };

    // The only reason why const char* is involved here is because std::string basically sees everything as a string ctor so overloading with it was almost impossible
    struct ObjParser {
        ObjParser(const char* delimiter, const char* entryDelimiter = "");
        ObjParser(const std::vector<std::string>& tupleKeys, const char* delimiter = "", const char* entryDelimiter = "");
        virtual json parse(const std::string& str) const;
    private:
        ObjectType m_bodyType;
        SeparatorType m_separatorType;
        std::string m_delimiter;
        std::string m_entryDelimiter;
        std::vector<std::string> m_tupleKeys;

        static std::vector<std::string> split(const std::string& str, const std::string& delimiter);

        json parseEntry(const std::string& str) const;
    };

    struct Parser : public ObjParser {
        Parser(const char* delimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
        Parser(const std::vector<std::string>& tupleKeys, const char* delimiter = "", const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
        Parser(const char* delimiter, const char* entryDelimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
        Parser(const std::vector<std::string>& tupleKeys, const char* delimiter, const char* entryDelimiter, const std::vector<std::tuple<std::string, ObjParser>>& metadataKeys = {});
        json parse(const std::string& str) const override;
    private:
        std::vector<std::tuple<std::string, ObjParser>> m_metadata;
    };

    static const std::unordered_map<std::string, Parser> parsers;

    bool canConvert(const std::string& path, const std::string& original) override;
    json convert(const std::string& path, const std::string& original) override;
};