#include "HoverNode.hpp"

HoverNode::HoverNode() : m_hovered(false) { }

bool HoverNode::init() {
    if (!CCNode::init()) {
        return false;
    }

    this->scheduleUpdate();

    return true;
}

float HoverNode::getTrueScale() {
    return this->getRecursiveScale(this, 1.0f);
}

float HoverNode::getRecursiveScale(CCNode* node, const float accumulator) {
    if (node->getParent()) {
        return this->getRecursiveScale(node->getParent(), accumulator * node->getScale());
    } else {
        return accumulator;
    }
}

void HoverNode::update(float delta) {
    const CCPoint mousePos = geode::cocos::getMousePos();
    const CCPoint truePos = this->convertToWorldSpace(this->getAnchorPoint());
    const CCPoint oppositeTruePos = truePos + this->getContentSize() * this->getTrueScale();

    if (
        mousePos.x >= truePos.x &&
        mousePos.y >= truePos.y &&
        mousePos.x <= oppositeTruePos.x &&
        mousePos.y <= oppositeTruePos.y
    ) {
        if (!m_hovered) {
            m_hovered = true;

            this->onHover();
        }
    } else if (m_hovered) {
        m_hovered = false;

        this->unHover();
    }
}