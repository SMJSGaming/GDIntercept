#pragma once

#include "../include.hpp"

using namespace geode::prelude;

struct DynamicEnumValue {
    bool operator==(const DynamicEnumValue& other) const;
    operator std::string() const;

    std::string value;

    DynamicEnumValue() = default;
    DynamicEnumValue(std::string value);
    DynamicEnumValue(const DynamicEnumValue& other);
};

template<>
struct matjson::Serialize<DynamicEnumValue> {
    static matjson::Value to_json(const DynamicEnumValue& value);
    static DynamicEnumValue from_json(const matjson::Value& json);
    static bool is_json(const matjson::Value& json);
};

class DynamicEnum : public SettingBaseValueV3<DynamicEnumValue> {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string key, std::string modID, const matjson::Value& json);
    static void reloadDynamicEnums(std::string sprSettingKey);
    static void registerLoader(std::string sprSettingKey, const std::function<void()>& loader);

    SettingNodeV3* createNode(const float width) override;
    std::string getSaveValue() const;
private:
    static std::unordered_map<std::string, Stream<std::function<void()>>> LOADERS;

    std::string m_saveValue;
};

template <>
struct SettingTypeForValueType<DynamicEnumValue> {
    using SettingType = DynamicEnum;
};

class DynamicEnumNode : public SettingValueNodeV3<DynamicEnum> {
public:
    static DynamicEnumNode* create(const std::shared_ptr<DynamicEnum>& setting, const float width);

    bool init(const std::shared_ptr<DynamicEnum>& setting, const float width);
    void updateState(CCNode* invoker) override;
    void onArrow(CCObject* sender);
    void onRefresh(CCObject* sender);
private:
    CCLabelBMFont* m_label;
    CCSprite* m_arrowLeftSpr;
    CCSprite* m_arrowRightSpr;
};