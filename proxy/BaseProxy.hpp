#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "Enums.hpp"
#include "../lib/json.hpp"

namespace proxy {
    #define PROXY_GETTER(type, name, capital_name) \
        public: type get##capital_name() const { return m_##name; } \
        private: type m_##name
}