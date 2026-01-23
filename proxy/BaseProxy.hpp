#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "Enums.hpp"
#include "../lib/json.hpp"
#include "../lib/Stream.hpp"

namespace proxy {
    #define PROXY_GETTER(type, name, capital_name) \
        public: const type& get##capital_name() const noexcept { return m_##name; } \
        protected: type m_##name
    #define PROXY_PRIMITIVE_GETTER(type, name, capital_name) \
        public: type get##capital_name() const noexcept { return m_##name; } \
        protected: type m_##name
}