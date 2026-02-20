#pragma once

#define OPT(expr) if (auto _opt_ = expr) _opt_
#define ESCAPE_WHEN(expr, ret) if (expr) return ret
#define CONTINUE_WHEN(expr) if (expr) continue
#define BREAK_WHEN(expr) if (expr) break

#define __GETTER(type, name, capital_name, prefix) type prefix##capital_name() const noexcept { return m_##name; }

#define GETTER(type, name, capital_name) \
    public: __GETTER(const type&, name, capital_name, get) \
    private: type m_##name
#define PRIMITIVE_GETTER(type, name, capital_name) \
    public: __GETTER(type, name, capital_name, get) \
    private: type m_##name
#define BOOL_GETTER(name, capital_name) \
    public: __GETTER(bool, name, capital_name, is) \
    private: bool m_##name
#define PROTECTED_GETTER(type, name, capital_name) \
    public: __GETTER(const type&, name, capital_name, get) \
    protected: type m_##name
#define PRIMITIVE_PROTECTED_GETTER(type, name, capital_name) \
    public: __GETTER(type, name, capital_name, get) \
    protected: type m_##name
#define BOOL_PROTECTED_GETTER(name, capital_name) \
    public: __GETTER(bool, name, capital_name, is) \
    protected: bool m_##name