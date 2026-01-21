#include "DynamicEnum.hpp"

bool DynamicEnumValue::operator==(const DynamicEnumValue& other) const {
    return value == other.value;
}

DynamicEnumValue::operator std::string() const {
    return value;
}

DynamicEnumValue::DynamicEnumValue(const std::string& value) : value(value) { }

DynamicEnumValue::DynamicEnumValue(const DynamicEnumValue& other) : value(other.value) { }

matjson::Value matjson::Serialize<DynamicEnumValue>::to_json(const DynamicEnumValue& value) {
    return value.value;
}

DynamicEnumValue matjson::Serialize<DynamicEnumValue>::from_json(const matjson::Value& json) {
    Result<std::string> result = json.asString();

    if (result) {
        return DynamicEnumValue(result.unwrap());
    } else {
        return DynamicEnumValue();
    }
}

bool matjson::Serialize<DynamicEnumValue>::is_json(const matjson::Value& json) {
    return json.isString();
}

std::unordered_map<std::string, Stream<std::function<void()>>> DynamicEnum::LOADERS;

Result<std::shared_ptr<SettingV3>> DynamicEnum::parse(const std::string& key, const std::string& modID, const matjson::Value& json) {
    std::shared_ptr<DynamicEnum> res = std::make_shared<DynamicEnum>();
    JsonExpectedValue root = checkJson(json, "DynamicEnum");

    res->parseBaseProperties(key, modID, root);
    root.needs("save-value").into(res->m_saveValue);
    root.checkUnknownKeys();

    return root.ok(std::static_pointer_cast<SettingV3>(res));
}

void DynamicEnum::reloadDynamicEnums(const std::string& sprSettingKey) {
    DynamicEnum::LOADERS.at(sprSettingKey).forEach([](const auto& loader) { loader(); });
}

void DynamicEnum::registerLoader(const std::string& sprSettingKey, const std::function<void()>& loader) {
    if (!DynamicEnum::LOADERS.contains(sprSettingKey)) {
        DynamicEnum::LOADERS[sprSettingKey] = {};
    }

    DynamicEnum::LOADERS.at(sprSettingKey).push_back(loader);
}

SettingNodeV3* DynamicEnum::createNode(const float width) {
    return DynamicEnumNode::create(std::static_pointer_cast<DynamicEnum>(shared_from_this()), width);
}

std::string DynamicEnum::getSaveValue() const {
    return m_saveValue;
}

DynamicEnumNode* DynamicEnumNode::create(const std::shared_ptr<DynamicEnum>& setting, const float width) {
    DynamicEnumNode* instance = new DynamicEnumNode();

    if (instance && instance->init(setting, width)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool DynamicEnumNode::init(const std::shared_ptr<DynamicEnum>& setting, const float width) {
    if (!SettingValueNodeV3::init(setting, width)) {
        return false;
    }

    CCMenu* menu = this->getButtonMenu();
    m_label = CCLabelBMFont::create(this->getSetting()->getValue().value.c_str(), "bigFont.fnt");
    m_arrowLeftSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    m_arrowRightSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    CCSprite* refreshSpr = CCSprite::createWithSpriteFrameName("GJ_getSongInfoBtn_001.png");
    CCMenuItemSpriteExtra* refreshBtn = CCMenuItemSpriteExtra::create(refreshSpr, this, menu_selector(DynamicEnumNode::onRefresh));
    CCMenuItemSpriteExtra* arrowLeftBtn = CCMenuItemSpriteExtra::create(m_arrowLeftSpr, this, menu_selector(DynamicEnumNode::onArrow));
    CCMenuItemSpriteExtra* arrowRightBtn = CCMenuItemSpriteExtra::create(m_arrowRightSpr, this, menu_selector(DynamicEnumNode::onArrow));

    m_arrowLeftSpr->setFlipX(true);
    m_arrowLeftSpr->setScale(0.4f);
    m_arrowRightSpr->setScale(0.4f);
    refreshSpr->setScale(0.6f);
    arrowLeftBtn->setTag(-1);
    arrowRightBtn->setTag(1);

    menu->addChildAtPosition(m_label, Anchor::Center);
    menu->addChildAtPosition(arrowLeftBtn, Anchor::Left, ccp(5, 0));
    menu->addChildAtPosition(arrowRightBtn, Anchor::Right, ccp(-5, 0));
    menu->addChildAtPosition(refreshBtn, Anchor::Left, ccp(-20, 0));

    this->updateState(nullptr);

    return true;
}

void DynamicEnumNode::updateState(CCNode* invoker) {
    SettingValueNodeV3::updateState(invoker);
    const bool enabled = this->getSetting()->shouldEnable();
    
    if (invoker != m_label) {
        m_label->setCString(this->getValue().value.c_str());
    }

    m_label->limitLabelWidth(88, 0.44f, 0.05f);
    m_arrowLeftSpr->setOpacity(enabled ? 255 : 100);
    m_arrowLeftSpr->setColor(enabled ? ccWHITE : ccGRAY);
    m_arrowRightSpr->setOpacity(enabled ? 255 : 100);
    m_arrowRightSpr->setColor(enabled ? ccWHITE : ccGRAY);
}

void DynamicEnumNode::onArrow(CCObject* sender) {
    const std::vector<std::string> options = Mod::get()->getSavedValue<std::vector<std::string>>(this->getSetting()->getSaveValue());
    size_t index = ranges::indexOf(options, this->getValue().value).value_or(0);

    if (sender->getTag() > 0) {
        index = index < options.size() - 1 ? index + 1 : 0;
    } else {
        index = index > 0 ? index - 1 : options.size() - 1;
    }

    this->setValue(options.at(index), typeinfo_cast<CCNode*>(sender));
}

void DynamicEnumNode::onRefresh(CCObject* sender) {
    const std::shared_ptr<DynamicEnum> setting = this->getSetting();

    m_label->setCString(this->getValue().value.c_str());

    DynamicEnum::reloadDynamicEnums(fmt::format("{}/{}", setting ->getModID(), setting->getKey()));
}

$execute {
    (void) Mod::get()->registerCustomSettingType("dynamic-enum", &DynamicEnum::parse);
}