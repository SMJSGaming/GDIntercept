#include "CategoryCell.hpp"

using namespace SideBarCell;

bool SideBarCategory::operator ==(const SideBarCategory& other) const {
    return icon == other.icon && name == other.name;
}

CategoryCell* SideBarCategory::toCategoryCell() const {
    return CategoryCell::create(*this);
}

CategoryCell* CategoryCell::create(const SideBarCategory& view) {
    CategoryCell* instance = new CategoryCell();

    if (instance && instance->init(view)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CategoryCell::init(const SideBarCategory& view) {
    const Theme::Theme theme = Theme::getTheme();

    ESCAPE_WHEN(!CCLayerColor::initWithColor(theme.menu.categoryBackground), false);
    ESCAPE_WHEN(!SharedWidthNode::init(), false);

    CCLabelBMFont* referenceSizeNode = theme.menu.font.createLabel("0");
    const float referenceHeight = cocos::getChild(referenceSizeNode, 0)->getContentHeight() * theme.menu.font.fontScale;
    const float height = referenceSizeNode->getScaledContentHeight() + theme.menu.font.lineHeight;
    const CCSize referenceSize = { referenceHeight, referenceHeight };
    CCSprite* icon = CCSprite::createWithSpriteFrameName(view.icon.c_str());
    m_icon = RescalingNode::create(icon, referenceSize);
    m_name = CenterLabel::create(view.name, theme.menu.font.fontName);

    theme.menu.icons.categoryIcon.applyTo(icon);
    m_icon->setAnchorPoint(CENTER_LEFT);
    m_icon->setPositionY(height / 2);
    theme.menu.categoryText.applyTo(m_name);
    m_name->setScale(theme.menu.font.fontScale);
    m_name->setAnchorPoint(CENTER_LEFT);
    m_name->setPosition({ theme.menu.paddingLeft, height / 2 });

    this->setContentHeight(height);
    this->addChild(m_icon);
    this->addChild(m_name);

    m_minWidth = referenceSize.width + theme.menu.paddingLeft + theme.menu.paddingRight;
    m_maxWidth = m_icon->getPositionX() + m_icon->getScaledContentWidth() + theme.menu.paddingRight;

    return true;
}

void CategoryCell::onHover() {
    // Do nothing
}

void CategoryCell::onScaleToMin() {
    m_name->setVisible(false);
    m_icon->setAnchorPoint(CENTER);
    m_icon->setPositionX(m_minWidth / 2);
}

void CategoryCell::onScaleToMax() {
    m_name->setVisible(true);
    m_icon->setAnchorPoint(CENTER_RIGHT);
    m_icon->setPositionX(m_maxWidth - Theme::getTheme().menu.paddingRight);
}

void CategoryCell::draw() {
    const Theme::Theme theme = Theme::getTheme();
    const CCSize size = this->getContentSize();

    SharedWidthNode::draw();

    ccDrawColor4B(theme.menu.categoryBorder);
    ccDrawLine(ZERO_POINT, { size.width, 0 });
    ccDrawLine({ 0, size.height }, { size.width, size.height });

}