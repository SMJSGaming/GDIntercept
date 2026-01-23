#pragma once

#include "Converter.hpp"

namespace proxy::converters {
    class RobTopToJsonV2 : public Converter {
    public:
        RobTopToJsonV2();
        bool canConvert(const std::string_view path, const bool isBody, const std::string_view original) const override;
        std::string convert(const std::string_view path, const std::string_view original) const override;
        std::string toRaw(const std::string_view path, const std::string_view original) const override;
    private:
        enum class ObjectType {
            AS_IS,
            LEVEL_RESPONSE,
            LEVEL,
            CREATOR,
            SONGS,
            SONG
        };

        class Object {
        public:
            static const std::string METADATA_KEY;

            Object(std::string_view delimiter);
            Object(std::string_view entryDelimiter, const ObjectType entryType);
            Object(std::string_view delimiter, const std::vector<std::string>& tuple);
            Object(std::string_view entryDelimiter, const std::vector<std::pair<std::string, ObjectType>>& tuple);
        };

        static const std::unordered_map<ObjectType, Object> PARSERS;
        static const std::unordered_map<std::string, ObjectType> ENDPOINT_MAPPINGS;
    };
}