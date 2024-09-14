#pragma once

#include "../../../include.hpp"

template<Node T = CCNode>
class SharedWidthNode : public T {
public:
    SharedWidthNode() : m_minWidth(0), m_maxWidth(0) { }

    void scaleToMin() {
        this->setContentWidth(m_minWidth);
        this->onScaleToMin();
    }

    void scaleToMax() {
        this->setContentWidth(m_maxWidth);
        this->onScaleToMax();
    }

    float getMinWidth() const {
        return m_minWidth;
    }

    float getMaxWidth() const {
        return m_maxWidth;
    }

    void overrideWidths(const float minWidth, const float maxWidth) {
        m_minWidth = minWidth;
        m_maxWidth = maxWidth;
    }
protected:
    float m_minWidth;
    float m_maxWidth;

    virtual void onScaleToMin() { }
    virtual void onScaleToMax() { }
};