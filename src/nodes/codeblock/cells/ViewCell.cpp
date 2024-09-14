#include "ViewCell.hpp"

using namespace SideBarCell;

ViewCell* SideBarView::toViewCell() const {
    return ViewCell::create(*this);
}

ViewCell* ViewCell::create(const SideBarView& view) {
    ViewCell* instance = new ViewCell();

    if (instance && instance->init(view)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool ViewCell::init(const SideBarView& view) {
    const ThemeStyle& theme = ThemeStyle::getTheme();

    ESCAPE_WHEN(!HoverNode<CCLayerColor>::init(), false);
    ESCAPE_WHEN(!CCLayerColor::initWithColor(theme.ui.menu.foreground), false);

    m_icon = Character::create(view.icon, theme.font.fontName);

    m_icon->setScale(theme.font.fontScale);

    const CCSize iconSize = m_icon->getScaledContentSize();
    const float height = iconSize.height + theme.font.lineHeight * 1.2f;
    m_name = CenterLabel::create(view.name.c_str(), theme.font.fontName);

    m_icon->setColor(view.iconColor);
    m_icon->setAnchorPoint(CENTER_LEFT);
    m_icon->setPositionY(height / 2);
    m_name->setColor(theme.syntax.text);
    m_name->setOpacity(theme.syntax.text);
    m_name->setScale(theme.font.fontScale);
    m_name->setAnchorPoint(CENTER_LEFT);
    m_name->setPosition({ PADDING + 2 + iconSize.width, height / 2 });

    this->setOpacity(0);
    this->setContentHeight(height);
    this->addChild(m_icon);
    this->addChild(m_name);

    m_minWidth = iconSize.width + PADDING * 2;
    m_maxWidth = m_name->getScaledContentWidth() + m_name->getPositionX() + PADDING;

    return true;
}

void ViewCell::onHover() {
    if (this->getOpacity() != 255) {
        this->setOpacity(160);
    }
}

void ViewCell::unHover() {
    if (this->getOpacity() != 255) {
        this->setOpacity(0);
    }
}

void ViewCell::onScaleToMin() {
    m_name->setVisible(false);
    m_icon->setAnchorPoint(CENTER);
    m_icon->setPositionX(m_minWidth / 2);
}

void ViewCell::onScaleToMax() {
    m_name->setVisible(true);
    m_icon->setAnchorPoint(CENTER_LEFT);
    m_icon->setPositionX(PADDING);
}