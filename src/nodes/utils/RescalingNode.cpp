#include "RescalingNode.hpp"

RescalingNode* RescalingNode::create(CCNode* node, const CCSize& size) {
    RescalingNode* instance = new RescalingNode();

    if (instance && instance->init(node, size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

RescalingNode::RescalingNode() : m_node(nullptr), m_rescaleWidth(true), m_rescaleHeight(true) { }

bool RescalingNode::init(CCNode* node, const CCSize& size) {
    ESCAPE_WHEN(!CCNode::init(), false);

    this->setNode(node);
    this->setContentSize(size);

    return true;
}

void RescalingNode::setNode(CCNode* node) {
    const CCSize size = this->getContentSize();

    OPT(m_node)->removeFromParent();
    m_node = node;

    m_node->setAnchorPoint(CENTER);
    m_node->ignoreAnchorPointForPosition(false);
    m_node->setPosition(size / 2);
    this->addChild(m_node);
    this->setContentSize(size);
}

void RescalingNode::setRescaleWidth(const bool value) {
    m_rescaleWidth = value;

    this->setContentSize(this->getContentSize());
}

void RescalingNode::setRescaleHeight(const bool value) {
    m_rescaleHeight = value;

    this->setContentSize(this->getContentSize());
}

void RescalingNode::setContentSize(const CCSize& size) {
    CCNode::setContentSize(size);

    m_node->setPosition(size / 2);

    if (m_rescaleWidth && m_rescaleHeight) {
        m_node->setScale(size.width / m_node->getContentWidth(), size.height / m_node->getContentHeight());
    } else if (m_rescaleWidth) {
        m_node->setScale(size.width / m_node->getContentWidth());
    } else if (m_rescaleHeight) {
        m_node->setScale(size.height / m_node->getContentHeight());
    }
}