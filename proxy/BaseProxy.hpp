#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "Enums.hpp"
#include "../lib/json.hpp"
#include "../lib/Stream.hpp"

namespace proxy {
    #define PROXY_GETTER(type, name, capital_name) \
        public: type get##capital_name() const { return m_##name; } \
        protected: type m_##name
    #define PROXY_BOOL_GETTER(name, capital_name) \
        public: bool is##capital_name() const { return m_##name; } \
        protected: bool m_##name
}