#pragma once

#define OPT(expr) if (auto _opt_ = expr) _opt_
#define ESCAPE_WHEN(expr, ret) if (expr) return ret
#define CONTINUE_WHEN(expr) if (expr) continue
#define BREAK_WHEN(expr) if (expr) break

#define __GETTER(type, name, capital_name) type get##capital_name() const { return m_##name; }
#define __SETTER(type, name, capital_name) void set##capital_name(type name) { m_##name = name; }

#define GETTER(type, name, capital_name) \
        public: __GETTER(type, name, capital_name) \
        private: type m_##name
#define PROTECTED_GETTER(type, name, capital_name) \
        public: __GETTER(type, name, capital_name) \
        protected: type m_##name

#define SETTER(type, name, capital_name) \
        public: __SETTER(type, name, capital_name) \
        private: type m_##name
#define PROTECTED_SETTER(type, name, capital_name) \
        public: __SETTER(type, name, capital_name) \
        protected: type m_##name

#define GETTER_SETTER(type, name, capital_name) \
    public: \
        __GETTER(type, name, capital_name) \
        __SETTER(type, name, capital_name) \
    private: \
        type m_##name
#define PROTECTED_GETTER_SETTER(type, name, capital_name) \
    public: \
        __GETTER(type, name, capital_name) \
        __SETTER(type, name, capital_name) \
    protected: \
        type m_##name