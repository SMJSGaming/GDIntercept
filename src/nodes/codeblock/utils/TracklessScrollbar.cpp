#include "TracklessScrollbar.hpp"

TracklessScrollbar* TracklessScrollbar::create(const bool vertical, const CCSize& size, CCScrollLayerExt* scrollLayer) {
    TracklessScrollbar* scrollbar = new TracklessScrollbar(vertical);

    if (scrollbar && scrollbar->init(size, scrollLayer)) {
        scrollbar->autorelease();

        return scrollbar;
    } else {
        CC_SAFE_DELETE(scrollbar);

        return nullptr;
    }
}

TracklessScrollbar::TracklessScrollbar(const bool vertical) : Scrollbar(), m_vertical(vertical) { }

bool TracklessScrollbar::init(const CCSize& size, CCScrollLayerExt* scrollLayer) {
    ESCAPE_WHEN(!Scrollbar::init(scrollLayer), false);

    m_track->removeFromParentAndCleanup(true);
    m_thumb->removeFromParentAndCleanup(true);
    this->addChild(m_thumb = CCScale9Sprite::createWithSpriteFrameName("d_largeSquare_01_001.png"));
    m_thumb->setScale(0.01f);
    m_thumb->setColor(ThemeStyle::getTheme().ui.scrollbar.thumb);
    m_thumb->setAnchorPoint(m_vertical ? TOP_LEFT : BOTTOM_RIGHT);
    this->setContentSize({ size.width, size.height });
    this->scheduleUpdate();

    return true;
}

void TracklessScrollbar::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    const CCSize size = this->getContentSize();
    const CCSize targetSize = m_target->getContentSize();
    const CCSize targetContentSize = m_target->m_contentLayer->getContentSize();
    const CCPoint touchLocation = this->convertTouchToNodeSpace(touch);
    const CCSize thumbSize = m_thumb->getScaledContentSize();

    if (m_vertical) {
        const float thumbY = std::min(std::max(0.0f, touchLocation.y - thumbSize.height / 2), size.height - thumbSize.height);

        m_target->m_contentLayer->setPositionY(-std::max(0.0f, targetContentSize.height - targetSize.height) * thumbY / (size.height - thumbSize.height));
    } else {
        const float thumbX = std::min(std::max(0.0f, touchLocation.x - thumbSize.width / 2), size.width - thumbSize.width);

        m_target->m_contentLayer->setPositionX(-std::max(0.0f, targetContentSize.width - targetSize.width) * thumbX / (size.width - thumbSize.width));
    }
}

void TracklessScrollbar::update(const float dt) {
    const CCSize size = this->getContentSize();
    const CCSize targetSize = m_target->getContentSize();
    const CCSize targetContentSize = m_target->m_contentLayer->getContentSize();

    Scrollbar::update(dt);

    if (m_vertical) {
        const float listDelta = std::max(targetContentSize.height - targetSize.height, 0.0f);
        const float thumbYPercentage = std::min(1.0f, std::max(0.0f, (listDelta + m_target->m_contentLayer->getPositionY()) / listDelta));

        m_thumb->setScaledContentSize({ size.width, std::min(targetSize.height / targetContentSize.height * size.height, size.height) });
        m_thumb->setPositionY(size.height - (size.height - m_thumb->getScaledContentHeight()) * thumbYPercentage);
    } else {
        const float listDelta = std::max(targetContentSize.width - targetSize.width, 0.0f);
        const float thumbXPercentage = std::min(1.0f, std::max(0.0f, (listDelta + m_target->m_contentLayer->getPositionX()) / listDelta));

        m_thumb->setScaledContentSize({ std::min(targetSize.width / targetContentSize.width * size.width, size.width), size.height });
        m_thumb->setPositionX(size.width - (size.width - m_thumb->getScaledContentWidth()) * thumbXPercentage);
    }

    if (m_touchDown) {
        m_thumb->setOpacity(200);
    } else {
        m_thumb->setOpacity(120);
    }
}

void TracklessScrollbar::draw() {
    const CCSize size = this->getContentSize();

    ccDrawColor4B(ThemeStyle::getTheme().ui.scrollbar.border);
    glLineWidth(2);

    if (m_vertical) {
        ccDrawLine(ZERO_POINT, { 0, size.height });
    } else {
        ccDrawLine({ 0, size.height }, size);
    }
}