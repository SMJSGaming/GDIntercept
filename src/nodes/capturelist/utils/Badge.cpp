#include "Badge.hpp"

Badge* Badge::create(const std::string& name, CCNode* node, const std::function<void(GLubyte)>& opacityCallback) {
    Badge* instance = new Badge(name, node, opacityCallback);

    if (instance && instance->init()) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

Badge::Badge(const std::string& name, CCNode* node, const std::function<void(GLubyte)>& opacityCallback) : HoverNode(),
m_name(name),
m_node(node),
m_tooltip(nullptr),
m_opacityCallback(opacityCallback) { }

bool Badge::init() {
    ESCAPE_WHEN(!HoverNode::init(), false);

    m_node->setAnchorPoint(ZERO_POINT);
    m_node->setPosition(ZERO_POINT);
    this->setContentSize(m_node->getScaledContentSize());
    this->addChild(m_node);

    return true;
}

void Badge::onHover() {
    ESCAPE_WHEN(m_tooltip,);

    CCPoint position = this->convertToWorldSpace(this->getAnchorPoint());
    CCSize contentSize = this->getContentSize() * this->getTrueScale();
    m_tooltip = Tooltip::create(m_name, 0.2f, 200);

    m_tooltip->setID("badge-tooltip"_spr);
    m_tooltip->setAnchorPoint(BOTTOM_CENTER);
    m_tooltip->setPosition(position + ccp(contentSize.width / 2, contentSize.height + 2));
    m_tooltip->show();

    m_opacityCallback(123);
}

void Badge::unHover() {
    OPT(m_tooltip)->removeFromParent();
    m_tooltip = nullptr;

    m_opacityCallback(255);
}