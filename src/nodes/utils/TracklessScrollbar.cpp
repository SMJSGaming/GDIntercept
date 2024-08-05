#include "TracklessScrollbar.hpp"

TracklessScrollbar* TracklessScrollbar::create(const CCSize& size, CCScrollLayerExt* scrollLayer) {
    TracklessScrollbar* scrollbar = new TracklessScrollbar();

    if (scrollbar && scrollbar->init(size, scrollLayer)) {
        scrollbar->autorelease();

        return scrollbar;
    } else {
        CC_SAFE_DELETE(scrollbar);

        return nullptr;
    }
}

bool TracklessScrollbar::init(const CCSize& size, CCScrollLayerExt* scrollLayer) {
    if (!Scrollbar::init(scrollLayer)) {
        return false;
    }

    m_track->removeFromParentAndCleanup(true);
    m_thumb->removeFromParentAndCleanup(true);
    this->addChild(m_thumb = CCScale9Sprite::createWithSpriteFrameName("d_largeSquare_01_001.png"));
    m_thumb->setScale(0.01f);
    m_thumb->setColor(ThemeStyle::getTheme().lineNum);
    m_thumb->setAnchorPoint(TOP_LEFT);
    this->setContentSize({ size.width, size.height });
    this->scheduleUpdate();

    return true;
}

void TracklessScrollbar::update(float delta) {
    const CCSize& size = this->getContentSize();
    const float targetHeight = m_target->getContentHeight();
    const float targetContentHeight = m_target->m_contentLayer->getContentHeight();
    const float listDelta = std::max(targetContentHeight - targetHeight, 0.0f);

    Scrollbar::update(delta);

    m_thumb->setScaledContentSize({ size.width, std::min(targetHeight / targetContentHeight * size.height, size.height) });
    m_thumb->setPosition({
        0,
        size.height - (
            // Scrollable area
            size.height - m_thumb->getContentHeight() * m_thumb->getScale()
        ) * std::min(std::max(0.0f, (listDelta + m_target->m_contentLayer->getPositionY()) / listDelta), 1.0f)
    });

    if (m_touchDown) {
        m_thumb->setOpacity(200);
    } else {
        m_thumb->setOpacity(120);
    }
}

void TracklessScrollbar::draw() {
    // Do nothing
}