#pragma once

#include "../../include.hpp"

// What can I say? I like it raw
struct BinaryToRaw {
    bool canConvert(const std::string& original);
    std::string convert(const std::string& original);
};