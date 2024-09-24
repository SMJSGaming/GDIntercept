#pragma once

#include "../../include.hpp"

template<Node T = CCNode>
class HoverNode : public T {
protected:
    HoverNode() : m_hovered(false) { };

    virtual bool init() override {
        ESCAPE_WHEN(!CCNode::init(), false);

        #ifndef GEODE_IS_ANDROID
            this->scheduleUpdate();
        #endif

        return true;
    };

    float getTrueScale() {
        return this->getRecursiveScale(this, 1.0f);
    }

    virtual void onHover() = 0;
    virtual void unHover() { };
protected:
    bool m_hovered;
private:
    float getRecursiveScale(CCNode* node, const float accumulator) {
        if (node->getParent()) {
            return this->getRecursiveScale(node->getParent(), accumulator * node->getScale());
        } else {
            return accumulator;
        }
    }

    void update(float delta) override {
        // Just incase something forcefully schedules an update
        GEODE_ANDROID(return;)

        const CCPoint mousePos = geode::cocos::getMousePos();
        const CCPoint truePos = this->convertToWorldSpace(this->getAnchorPoint());
        const CCPoint oppositeTruePos = truePos + this->getContentSize() * this->getTrueScale();

        if (mousePos >= truePos && mousePos <= oppositeTruePos) {
            if (!m_hovered) {
                m_hovered = true;

                this->onHover();
            }
        } else if (m_hovered) {
            m_hovered = false;

            this->unHover();
        }
    }
};