#include "ActionCell.hpp"

using namespace SideBarCell;

SideBarActionButton::SideBarActionButton() : name(""),
icon("") { }

SideBarActionButton::SideBarActionButton(const std::string& name, const std::string& icon, const StandardCallback& callback) : name(name),
icon(icon),
callback(callback) { }

bool SideBarActionButton::isEmpty() const {
    return name.empty() || icon.empty();
}

SideBarAction::SideBarAction(const SideBarActionButton& on, const StandardCallback& enableState) : on(on), enableState(enableState) { }

SideBarAction::SideBarAction(
    const SideBarActionButton& on,
    const SideBarActionButton& off,
    const StandardCallback& determineState,
    const StandardCallback& enableState
) : on(on),
off(off),
determineState(determineState),
enableState(enableState) { }

SideBarAction::SideBarAction(
    const std::string& setting,
    const std::tuple<std::string, std::string>& names,
    const std::tuple<std::string, std::string>& icons,
    const bool inverse,
    const std::tuple<OverrideCallback, OverrideCallback>& overrides
) : on({
    std::get<0>(names),
    std::get<0>(icons),
    [setting, inverse, overrides](CodeBlock* block) {
        const auto& override = inverse ? std::get<1>(overrides) : std::get<0>(overrides);
        const auto callback = [setting, inverse]{ Mod::get()->setSettingValue(setting, !inverse); return true; };

        if (override) {
            return override(block, callback);
        } else {
            return callback();
        }
    }
}),
off({
    std::get<1>(names),
    std::get<1>(icons),
    [setting, inverse, overrides](CodeBlock* block) {
        const auto& override = inverse ? std::get<0>(overrides) : std::get<1>(overrides);
        const auto callback = [setting, inverse]{ Mod::get()->setSettingValue(setting, inverse); return true; };

        if (override) {
            return override(block, callback);
        } else {
            return callback();
        }
    }
}),
determineState([setting, inverse](CodeBlock* block) {
    return Mod::get()->getSettingValue<bool>(setting) == inverse;
}) { }

ActionCell* SideBarAction::toActionCell(CodeBlock* block) const {
    return ActionCell::create(block, *this);
}

ActionCell* ActionCell::create(CodeBlock* block, const SideBarAction& action) {
    ActionCell* instance = new ActionCell(block, action);

    if (instance && instance->init()) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

ActionCell::ActionCell(CodeBlock* block, const SideBarAction& action) : m_state(action.off.isEmpty() || action.determineState(block)),
m_action(action),
m_block(block),
m_enabled(true) { }

bool ActionCell::init() {
    const Theme::Theme theme = Theme::getTheme();

    ESCAPE_WHEN(!HoverNode::init(), false);
    ESCAPE_WHEN(!CCLayerColor::initWithColor(theme.menu.foreground), false);

    CCLabelBMFont* referenceSizeNode = theme.menu.font.createLabel("0");
    const bool oldState = m_state;
    const float referenceHeight = cocos::getChild(referenceSizeNode, 0)->getContentHeight() * theme.menu.font.fontScale;
    const float height = referenceSizeNode->getScaledContentHeight() + theme.menu.font.lineHeight;
    const CCSize referenceSize = { referenceHeight, referenceHeight };
    m_icon = RescalingNode::create(CCNode::create(), referenceSize);
    m_name = CenterLabel::create("", theme.menu.font.fontName);

    theme.menu.text.applyTo(m_name);
    m_name->setScale(theme.menu.font.fontScale);
    m_name->setAnchorPoint(CENTER_LEFT);
    m_name->setPosition({ theme.menu.paddingLeft, height / 2 });
    m_icon->setAnchorPoint(CENTER_LEFT);
    m_icon->setPosition({ theme.menu.paddingLeft + theme.menu.paddingCenter + m_name->getScaledContentWidth(), height / 2 });

    this->setOpacity(0);
    this->setContentHeight(height);
    this->addChild(m_icon);
    this->addChild(m_name);
    this->setState(!oldState);

    const float maxWidthOpposite = this->getMaxCellWidth();

    this->setState(oldState);

    m_icon->setPositionX(theme.menu.paddingLeft + theme.menu.paddingCenter + m_name->getScaledContentWidth());

    m_minWidth = referenceSize.width + theme.menu.paddingLeft + theme.menu.paddingRight;
    m_maxWidth = std::max(this->getMaxCellWidth(), maxWidthOpposite);

    return true;
}

void ActionCell::setState(const bool state) {
    m_state = state || m_action.off.isEmpty();
    const Theme::Theme theme = Theme::getTheme();
    const SideBarActionButton button = m_state ? m_action.on : m_action.off;
    CCSprite* icon = CCSprite::createWithSpriteFrameName(button.icon.c_str());

    m_name->setCString(button.name.c_str());
    m_icon->setNode(icon);
    theme.menu.icons.actionIcon.applyTo(icon);
}

float ActionCell::getMaxCellWidth() const {
    return m_icon->getPositionX() + m_icon->getScaledContentWidth() + Theme::getTheme().menu.paddingRight;
}

void ActionCell::hideOpacity(const float dt) {
    // Reset the hover state to reinitialize the opacity if needed
    m_hovered = false;

    this->setOpacity(0);
}

bool ActionCell::isEnabled() const {
    return m_enabled;
}

void ActionCell::enable() {
    const Theme::Theme theme = Theme::getTheme();
    CCSprite* icon = as<CCSprite*>(m_icon->getNode());

    m_enabled = true;

    theme.menu.icons.actionIcon.applyTo(icon);
    theme.menu.text.applyTo(m_name);
}

void ActionCell::disable() {
    const Theme::Theme theme = Theme::getTheme();
    CCSprite* icon = as<CCSprite*>(m_icon->getNode());

    m_enabled = false;

    theme.menu.icons.disabledIcon.applyTo(icon);
    theme.menu.disabledText.applyTo(m_name);
}

void ActionCell::onHover() {
    if (this->getOpacity() != 255 && m_enabled) {
        this->setOpacity(160);
    }
}

void ActionCell::unHover() {
    if (this->getOpacity() != 255) {
        this->setOpacity(0);
    }
}

void ActionCell::onScaleToMin() {
    m_name->setVisible(false);
    m_icon->setAnchorPoint(CENTER);
    m_icon->setPositionX(m_minWidth / 2);
}

void ActionCell::onScaleToMax() {
    m_name->setVisible(true);
    m_icon->setAnchorPoint(CENTER_RIGHT);
    m_icon->setPositionX(m_maxWidth - Theme::getTheme().menu.paddingRight);
}