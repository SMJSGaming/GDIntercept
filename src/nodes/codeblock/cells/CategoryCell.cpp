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
    const ThemeStyle& theme = ThemeStyle::getTheme();

    ESCAPE_WHEN(!CCLayerColor::init(), false);
    ESCAPE_WHEN(!SharedWidthNode::init(), false);

    CCLabelBMFont* referenceSizeNode = CCLabelBMFont::create("0", theme.font.fontName);
    const float referenceHeight = cocos::getChild(referenceSizeNode, 0)->getContentHeight() * theme.font.fontScale;
    const CCSize referenceSize = { referenceHeight, referenceHeight };
    const float height = referenceSizeNode->getContentHeight() * theme.font.fontScale + theme.font.lineHeight * 1.2f;
    CCSprite* icon = CCSprite::createWithSpriteFrameName(view.icon.c_str());
    m_icon = RescalingNode::create(icon, referenceSize);
    m_name = CCLabelBMFont::create(view.name.c_str(), theme.font.fontName);

    icon->setColor(theme.syntax.text);
    icon->setOpacity(theme.syntax.text);
    m_icon->setAnchorPoint(CENTER_LEFT);
    m_icon->setPositionY(height / 2);
    m_name->setColor(theme.syntax.text);
    m_name->setOpacity(theme.syntax.text);
    m_name->setScale(theme.font.fontScale);
    m_name->setAnchorPoint(CENTER_LEFT);
    m_name->setPosition({ PADDING, height / 2 });

    this->setContentHeight(height);
    this->addChild(m_icon);
    this->addChild(m_name);

    m_minWidth = referenceSize.width + PADDING * 2;
    m_maxWidth = m_icon->getPositionX() + m_icon->getScaledContentWidth() + PADDING;

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
    m_icon->setPositionX(m_maxWidth - PADDING);
}

void CategoryCell::draw() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const CCSize size = this->getContentSize();

    SharedWidthNode::draw();

    ccDrawColor4B(theme.ui.menu.border.a == 0 ? theme.ui.scrollbar.border : theme.ui.menu.border);
    ccDrawLine(ZERO_POINT, { size.width, 0 });
    ccDrawLine({ 0, size.height }, { size.width, size.height });

}