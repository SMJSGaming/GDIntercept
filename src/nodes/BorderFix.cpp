#include "BorderFix.hpp"

BorderFix* BorderFix::create(CCNode* node, const ccColor4B& backgroundColor, const CCSize& size, const CCPoint& padding) {
    BorderFix* instance = new BorderFix(padding);

    if (instance && instance->init(node, backgroundColor, size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

BorderFix::BorderFix(const CCPoint& padding) : m_padding({ padding.x, padding.y, padding.x, padding.y }) { }

bool BorderFix::init(const ccColor4B& backgroundColor, const CCSize& size) {
    return this->init(nullptr, backgroundColor, size);
}

bool BorderFix::init(CCNode* node, const ccColor4B& backgroundColor, const CCSize& size) {
    if (!this->initWithColor(backgroundColor)) {
        return false;
    }

    CCScale9Sprite* border = CCScale9Sprite::create("inverseborder.png"_spr);
    CCLayer* content = CCLayer::create();

    border->setID("border_sprite"_spr);
    border->setAnchorPoint({ 0, 0 });
    border->setPosition({ 0, 0 });
    content->setID("border_content"_spr);
    content->setContentSize(size);
    this->addChild(border, 1);
    this->addChild(content);

    if (node != nullptr) {
        this->setNode(node);
    }

    this->setSize(size);

    return true;
}

void BorderFix::setPadding(const CCPoint& padding) {
    this->setPadding(padding.x, padding.y);
}

void BorderFix::setPadding(float x, float y) {
    this->setPaddingX(x);
    this->setPaddingY(y);
}

void BorderFix::setPadding(float padding) {
    this->setPadding(padding, padding);
}

void BorderFix::setPaddingX(float x) {
    this->setPaddingLeft(x);
    this->setPaddingRight(x);
}

BorderFix::Padding BorderFix::getPadding() {
    return m_padding;
}

float BorderFix::getPaddingX() {
    return (m_padding.left + m_padding.right) / 2;
}

void BorderFix::setPaddingY(float y) {
    this->setPaddingTop(y);
    this->setPaddingBottom(y);
}

float BorderFix::getPaddingY() {
    return (m_padding.top + m_padding.bottom) / 2;
}

void BorderFix::setPaddingTop(float top) {
    m_padding.top = top;

    this->updatePadding();
}

float BorderFix::getPaddingTop() {
    return m_padding.top;
}

void BorderFix::setPaddingRight(float right) {
    m_padding.right = right;

    this->updatePadding();
}

float BorderFix::getPaddingRight() {
    return m_padding.right;
}

void BorderFix::setPaddingBottom(float bottom) {
    m_padding.bottom = bottom;

    this->updatePadding();
}

float BorderFix::getPaddingBottom() {
    return m_padding.bottom;
}

void BorderFix::setPaddingLeft(float left) {
    m_padding.left = left;

    this->updatePadding();
}

float BorderFix::getPaddingLeft() {
    return m_padding.left;
}

void BorderFix::setBackgroundColor(const ccColor4B& color) {
    this->setColor({ color.r, color.g, color.b });
    this->setOpacity(color.a);
}

ccColor4B BorderFix::getBackgroundColor() {
    return { _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity };
}

void BorderFix::setNode(CCNode* node) {
    CCNode* content = this->getChildByID("border_content"_spr);

    // Can't assume an ID as the node is a user input and may have its ID changed
    if (CCNode* oldNode = cocos::getChild<CCNode>(content, 0)) {
        // Not going to mess with releasing the node, I'll leave that to the user
        oldNode->removeFromParent();
    }

    content->addChild(node);

    this->updatePadding();
}

CCNode* BorderFix::getNode() {
    if (CCNode* node = cocos::getChild<CCNode>(this->getChildByID("border_content"_spr), 0)) {
        return node;
    } else {
        return nullptr;
    }
}

void BorderFix::setSize(const CCSize& size) {
    this->setContentSize(size);
    this->getChildByID("border_sprite"_spr)->setContentSize(size);
    this->getChildByID("border_content"_spr)->setContentSize(size);
    this->updatePadding();
}

void BorderFix::updatePadding() {
    if (CCNode* node = this->getNode()) {
        CCSize size = this->getContentSize();

        node->setAnchorPoint({ 0, 0 });
        node->setPosition({ m_padding.left, m_padding.bottom });
        node->setContentSize(size - ccp(this->getPaddingX(), this->getPaddingY()) * 2);
    }
}