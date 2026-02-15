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

    NineSlice* newThumb = NineSlice::createWithSpriteFrameName("d_largeSquare_01_001.png");

    this->getTrack()->removeFromParent();
    this->getThumb()->removeFromParent();
    this->setThumb(newThumb);
    this->addChild(newThumb);
    newThumb->setScale(0.01f);
    newThumb->setAnchorPoint(m_vertical ? TOP_LEFT : BOTTOM_RIGHT);
    this->setContentSize({ size.width, size.height });
    this->scheduleUpdate();

    return true;
}

void TracklessScrollbar::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    CCScrollLayerExt* target = this->getTarget();
    const CCSize& size = this->getContentSize();
    const CCSize& targetSize = target->getContentSize();
    const CCSize& targetContentSize = target->m_contentLayer->getContentSize();
    const CCPoint touchLocation = this->convertTouchToNodeSpace(touch);
    const CCSize thumbSize = this->getThumb()->getScaledContentSize();

    if (m_vertical) {
        const float remainingHeight = size.height - thumbSize.height;
        ESCAPE_WHEN(remainingHeight == 0,);
        const float thumbY = std::min(std::max(0.0f, touchLocation.y - thumbSize.height / 2), remainingHeight);

        target->m_contentLayer->setPositionY(-std::max(0.0f, targetContentSize.height - targetSize.height) * thumbY / remainingHeight);
    } else {
        const float remainingWidth = size.width - thumbSize.width;
        ESCAPE_WHEN(remainingWidth == 0,);
        const float thumbX = std::min(std::max(0.0f, touchLocation.x - thumbSize.width / 2), remainingWidth);

        target->m_contentLayer->setPositionX(-std::max(0.0f, targetContentSize.width - targetSize.width) * thumbX / remainingWidth);
    }
}

void TracklessScrollbar::update(const float dt) {
    CCScrollLayerExt* target = this->getTarget();
    NineSlice* thumb = this->getThumb();
    const Theme::Theme& theme = Theme::getTheme();
    const CCSize size = this->getContentSize();
    const CCSize targetSize = target->getContentSize();
    const CCSize targetContentSize = target->m_contentLayer->getContentSize();
    const CCSize thumbSize = thumb->getScaledContentSize();

    if (this->isTouching()) {
        theme.code.scrollbar.activeThumb.applyTo(thumb);
    } else {
        theme.code.scrollbar.thumb.applyTo(thumb);
    }

    ESCAPE_WHEN(m_vertical ?
        targetContentSize.height == targetSize.height && targetSize.height == thumbSize.height :
        targetContentSize.width == targetSize.width && targetSize.width == thumbSize.width,);
    Scrollbar::update(dt);

    if (m_vertical) {
        const float listDelta = std::max(targetContentSize.height - targetSize.height, 0.0f);
        const float thumbYPercentage = std::min(1.0f, std::max(0.0f, (listDelta + target->m_contentLayer->getPositionY()) / listDelta));

        thumb->setScaledContentSize({ size.width, std::min(targetSize.height / targetContentSize.height * size.height, size.height) });
        thumb->setPositionY(size.height - (size.height - thumbSize.height) * thumbYPercentage);
    } else {
        const float listDelta = std::max(targetContentSize.width - targetSize.width, 0.0f);
        const float thumbXPercentage = std::min(1.0f, std::max(0.0f, (listDelta + target->m_contentLayer->getPositionX()) / listDelta));

        thumb->setScaledContentSize({ std::min(targetSize.width / targetContentSize.width * size.width, size.width), size.height });
        thumb->setPositionX(size.width - (size.width - thumbSize.width) * thumbXPercentage);
    }
}

void TracklessScrollbar::draw() {
    const CCSize size = this->getContentSize();

    ccDrawColor4B(Theme::getTheme().code.scrollbar.border);
    glLineWidth(2);

    if (m_vertical) {
        ccDrawLine(ZERO_POINT, { 0, size.height });
    } else {
        ccDrawLine({ 0, size.height }, size);
    }
}