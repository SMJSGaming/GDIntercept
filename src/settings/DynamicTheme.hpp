#pragma once

#include "../include.hpp"

using namespace geode::prelude;

struct DynamicThemeValue {
    bool operator==(const DynamicThemeValue& other) const;
    operator std::string() const;

    std::string value;

    DynamicThemeValue() = default;
    DynamicThemeValue(std::string value);
    DynamicThemeValue(const DynamicThemeValue& other);
};

template<>
struct matjson::Serialize<DynamicThemeValue> {
    static matjson::Value to_json(const DynamicThemeValue& value);
    static DynamicThemeValue from_json(const matjson::Value& json);
    static bool is_json(const matjson::Value& json);
};

class DynamicTheme : public SettingBaseValueV3<DynamicThemeValue> {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string key, std::string modID, const matjson::Value& json);
    static void reloadDynamicThemes(std::string sprSettingKey);

    template<typename F> requires(std::invocable<F>)
    static void registerLoader(std::string sprSettingKey, F&& loader) {
        if (!DynamicTheme::LOADERS.contains(sprSettingKey)) {
            DynamicTheme::LOADERS[sprSettingKey] = {};
        }

        DynamicTheme::LOADERS.at(sprSettingKey).emplace_back(std::move(loader));
    }

    SettingNodeV3* createNode(const float width) override;
    std::string getSaveValue() const;
private:
    static StringMap<Stream<std::function<void()>>> LOADERS;

    std::string m_saveValue;
};

template <>
struct SettingTypeForValueType<DynamicThemeValue> {
    using SettingType = DynamicTheme;
};

class DynamicThemeNode : public SettingValueNodeV3<DynamicTheme> {
public:
    static DynamicThemeNode* create(const std::shared_ptr<DynamicTheme>& setting, const float width);

    bool init(const std::shared_ptr<DynamicTheme>& setting, const float width);
    void updateState(CCNode* invoker) override;
    void onArrow(CCObject* sender);
    void onRefresh(CCObject* sender);
    void onFiles(CCObject* sender);
    void onDocs(CCObject* sender);
private:
    CCLabelBMFont* m_label;
    CCSprite* m_arrowLeftSpr;
    CCSprite* m_arrowRightSpr;
};